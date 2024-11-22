#ifndef LED_H_
#define LED_H_

#define LED_GREEN  1
#define LED_ORANGE 2
#define LED_RED    3

void led_init_all(void);
void led_on(uint8_t led_no);
void led_off(uint8_t led_no);

#endif /* LED_H_ */
