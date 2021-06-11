/* 
 * File name: prgsem-mbed.cpp
 * Description: The main file for nucleo
 * Date: 05/05/2021
 * Author: Jakub Sukdol
 */

#include "mbed.h"
#include "messages_nucleo.h"

Serial ser(SERIAL_TX, SERIAL_RX);
DigitalOut myled(LED1);
Ticker ticker_led;
InterruptIn abort_btn(USER_BUTTON);


/* Hardware fcns */
void flip() {
    myled = !myled;
}

bool abort_comp = false;
bool computing = false;
void btn_pressed()
{
    abort_comp = true;
}
/* End of hardware fcns */

#define MSG_SIZE (sizeof(message))

#define VER_MAJOR 1
#define VER_MINOR 0
#define VER_PATCH 0

#define COMP_TIME 0.5
#define FLIP_PERIOD 0.09
#define INIT_FLIP_PERIOD 0.15

void Tx_interrupt();
void Rx_interrupt();
bool send_buffer(const uint8_t* msg, unsigned int size);
bool receive_message(uint8_t *msg_buf, int size, int *len);
bool send_message(const message *msg, uint8_t *buf, int size);
 
#define BUF_SIZE 255

#define BAUD_NUM 4    // number of baud levels
 
 // circular buffers
char tx_buffer[BUF_SIZE];
char rx_buffer[BUF_SIZE];
 
// pointers to the circular buffers
volatile int tx_in = 0;
volatile int tx_out = 0;
volatile int rx_in = 0;
volatile int rx_out = 0;

int fractal(double re, double im, int iter, double c_re, double c_im, int n);

int main()
{
    ser.baud(115200);
    ser.attach(&Rx_interrupt, Serial::RxIrq); // attach interrupt handler to receive data
    ser.attach(&Tx_interrupt, Serial::TxIrq); // attach interrupt handler to transmit data
    
    message msg = { .type = MSG_STARTUP, .data.startup.message = { 'F', 'R', 'A', 'C', 'T', 'A', 'L', '0', '1' } };
    uint8_t msg_buf[MSG_SIZE];
    int msg_len;
    
    while (ser.readable()) {
        // get rid of junk
        ser.getc();
    }
    
    // init msg
    send_message(&msg, msg_buf, msg_len);
    
    // 5* flip led as an indicator of initialization
    for (int i = 0; i < 10; ++i) {
        flip();
        wait(INIT_FLIP_PERIOD);
    }
    
    abort_btn.rise(&btn_pressed);
    
    struct {
        uint8_t chunk_id;
        uint8_t cursor_x;
        uint8_t cursor_y;
        uint8_t n;
        double c_re;
        double c_im;
        double d_re;
        double d_im;
        double cur_re; // cursor
        double cur_im;
        double re_min;
        double im_min;
        int n_re;
        int n_im;
        int task_id;
        int nbr_tasks;
        bool computing;
    } computation = { 0 };
    
    bool change_baud = false;
    int  baud_level = 0;
    
    // main while cycle
    while (1) {
        bool send = false;
        if (change_baud) {
            switch (baud_level) {
                case 0: // lowest baud -> make it higher
                ser.baud(230400);
                break;
                case 1: // go a bit faster
                ser.baud(460800);
                break;
                case 2: // over half mega baud
                ser.baud(576000);
                break;
                case 3: // go back to basic level
                ser.baud(115200);
                break;
            }
            baud_level += 1;        // update baud_level
            baud_level %= BAUD_NUM;
            change_baud = false;
            while (ser.readable()) {
                ser.getc();
            } // get rid of junk     
            wait(1);    
            
        }
        if (rx_in != rx_out) {
            if (receive_message(msg_buf, MSG_SIZE, &msg_len)) {
                if (parse_message_buf(msg_buf, msg_len, &msg)) {
                    switch (msg.type) {
                        case MSG_GET_VERSION:
                            msg.type = MSG_VERSION;
                            msg.data.version.major = VER_MAJOR;  
                            msg.data.version.minor = VER_MINOR; 
                            msg.data.version.patch = VER_PATCH; 
                            send = true;
                            break;
                        case MSG_SET_COMPUTE:
                            computation.c_re = msg.data.set_compute.c_re;
                            computation.c_im = msg.data.set_compute.c_im;
                            computation.d_re = msg.data.set_compute.d_re;
                            computation.d_im = msg.data.set_compute.d_im;
                            computation.n = msg.data.set_compute.n;
                            msg.type = MSG_OK;
                            send = true;
                            break;
                        case MSG_COMPUTE:
                            computation.task_id = 0;
                            computation.cursor_x = 0;
                            computation.cursor_y = 0;
                            computation.chunk_id = msg.data.compute.cid;
                            computation.re_min = computation.cur_re = msg.data.compute.re;
                            computation.im_min = computation.cur_im = msg.data.compute.im;
                            computation.n_re = msg.data.compute.n_re;
                            computation.n_im = msg.data.compute.n_im;
                            computation.nbr_tasks = computation.n_re * computation.n_im;
                            ticker_led.attach(&flip, FLIP_PERIOD);
                            abort_comp = false;
                            computation.computing = true;
                            break;
                        case MSG_ABORT:
                            msg.type = MSG_OK;
                            send = true;
                            computing = computation.computing = false;
                            ticker_led.detach();
                            myled = 0;
                            break;
                        case MSG_BAUD_CHANGE:
                            msg.type = MSG_BAUD_CHANGE; // verify baud change
                            send = true;
                            change_baud = true;
                            break;
                        case MSG_BAUD_RESET:
                            wait(2); // until app is closed
                            ser.baud(115200);
                            break;
                        default:                 
                            break;
                    }
                    if (send == true) {
                        if (!send_message(&msg, msg_buf, msg_len)) {
                            // indicate error with LED
                            myled = !myled;
                        }
                    }
                    if (change_baud) {  
                        wait(1);
                    }
                } else {
                    msg.type = MSG_ERROR;
                    send_message(&msg, msg_buf, msg_len);
                }
            } else {
                msg.type = MSG_ERROR;
                send_message(&msg, msg_buf, msg_len);    
            }
        } else {
            if (computation.computing) {
                if ((computation.task_id < computation.n_re * computation.n_im) && !abort_comp) {
                    msg.type = MSG_COMPUTE_DATA;
                    msg.data.compute_data.cid = computation.chunk_id;
                    msg.data.compute_data.i_re = computation.cursor_x;
                    msg.data.compute_data.i_im = computation.cursor_y;
                    msg.data.compute_data.iter = fractal(computation.cur_re, computation.cur_im, 0, computation.c_re, computation.c_im, computation.n);
                    
                    // update cursors
                    computation.cursor_x += 1;
                    computation.cur_re += computation.d_re;
                    if (computation.cursor_x == computation.n_re) {
                        computation.cursor_x = 0;
                        computation.cursor_y += 1;
                        computation.cur_re = computation.re_min;
                        computation.cur_im += computation.d_im;
                    }
                    computation.task_id += 1;
                    send_message(&msg, msg_buf, msg_len);
                } else {
                    computing = computation.computing = false;
                    if (abort_comp) {
                        msg.type = MSG_ABORT;
                        abort_comp = false;
                    }
                    else {
                        msg.type = MSG_DONE;
                    }
                    send_message(&msg, msg_buf, msg_len);
                    ticker_led.detach();
                    myled = 0;
                }
            } 
            sleep();
        }
    } // end of while (1) 
}

int fractal(double re, double im, int iter, double c_re, double c_im, int n)
{
    iter += 1;
    double re_old = re;
    re = pow(re, 2) - pow(im,2) + c_re;
    im = 2*re_old*im + c_im;

    double z_abs = (sqrt(pow(re, 2) + pow(im, 2)));
    if (iter == n || z_abs > 2) {
        return iter;
    } else {
        return fractal(re, im, iter, c_re, c_im, n);
    } 
}

void Tx_interrupt()
{
    // send a single byte as the interrupt is triggered on empty out buffer 
    if (tx_in != tx_out) {
        ser.putc(tx_buffer[tx_out]);
        tx_out = (tx_out + 1) % BUF_SIZE;
    } else { // buffer sent out, disable Tx interrupt
        USART2->CR1 &= ~USART_CR1_TXEIE; // disable Tx interrupt
    }
    return;
}

void Rx_interrupt()
{
    // receive bytes and stop if rx_buffer is full
    while ((ser.readable()) && (((rx_in + 1) % BUF_SIZE) != rx_out)) {
        rx_buffer[rx_in] = ser.getc();
        rx_in = (rx_in + 1) % BUF_SIZE;
    }
    return;
}

bool send_buffer(const uint8_t* msg, unsigned int size)
{
    if (!msg && size == 0) {
        return false;    // size must be > 0
    }
    int i = 0;
    NVIC_DisableIRQ(USART2_IRQn); // start critical section for accessing global data
    USART2->CR1 |= USART_CR1_TXEIE; // enable Tx interrupt on empty out buffer
    bool empty = (tx_in == tx_out);
    while ( (i == 0) || i < size ) { //end reading when message has been read
        if ( ((tx_in + 1) % BUF_SIZE) == tx_out) { // needs buffer space
            NVIC_EnableIRQ(USART2_IRQn); // enable interrupts for sending buffer
            while (((tx_in + 1) % BUF_SIZE) == tx_out) {
                /// let interrupt routine empty the buffer
            }
            NVIC_DisableIRQ(USART2_IRQn); // disable interrupts for accessing global buffer
        }
        tx_buffer[tx_in] = msg[i];
        i += 1;
        tx_in = (tx_in + 1) % BUF_SIZE;
    } // send buffer has been put to tx buffer, enable Tx interrupt for sending it out
    USART2->CR1 |= USART_CR1_TXEIE; // enable Tx interrupt
    NVIC_EnableIRQ(USART2_IRQn); // end critical section
    return true;
}

bool receive_message(uint8_t *msg_buf, int size, int *len)
{
    bool ret = false;
    int i = 0;
    *len = 0; // message size
    NVIC_DisableIRQ(USART2_IRQn); // start critical section for accessing global data
    while ( ((i == 0) || (i != *len)) && i < size ) {
        if (rx_in == rx_out) { // wait if buffer is empty
            NVIC_EnableIRQ(USART2_IRQn); // enable interrupts for receing buffer
            while (rx_in == rx_out) { // wait of next character
            }
            NVIC_DisableIRQ(USART2_IRQn); // disable interrupts for accessing global buffer
        }
        uint8_t c = rx_buffer[rx_out];
        if (i == 0) { // message type
            if (get_message_size(c, len)) { // message type recognized
                msg_buf[i++] = c;
                ret = *len <= size; // msg_buffer must be large enough
            } else {
                ret = false;
                break; // unknown message
            }
        } else {
            msg_buf[i++] = c;
        }
        rx_out = (rx_out + 1) % BUF_SIZE;
    }
    NVIC_EnableIRQ(USART2_IRQn); // end critical section
    return ret;
}

bool send_message(const message *msg, uint8_t *buf, int size)
{
    return fill_message_buf(msg, buf, MSG_SIZE, &size)
                        && send_buffer(buf, size);

}
