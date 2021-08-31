$(PROJECT_NAME).hex: clean $(PROJECT_NAME).out
		$(AVR_PATH)/avr-objcopy -j .text -j .data -O ihex $(PROJECT_NAME).out $(PROJECT_NAME).hex
  
$(PROJECT_NAME).S: $(PROJECT_NAME).out
		$(AVR_PATH)/avr-objdump -h -S $(PROJECT_NAME).out > $@

$(PROJECT_NAME).out: $(FILES)
		$(AVR_PATH)/avr-gcc -o$(PROJECT_NAME).out $(LFLAGS) $(FILES) 

%.o: %.c
		$(AVR_PATH)/avr-gcc $(CFLAGS) $(INCLUDES) -o $@ $?

%.o: %.S
		$(AVR_PATH)/avr-gcc $(CFLAGS) $(INCLUDES) -o $@ $?

upload: $(PROJECT_NAME).hex
		$(AVR_DUDE) \
				-C$(AVR_DUDE_CONF) \
				-v \
				-p$(MCU) \
				-c$(PROGRAMMER) \
				-P$(SERIAL_DEV) \
				-b$(BAUD) \
				$(DISABLE_AUTO_ERASE) \
				-Uflash:w:$(PROJECT_NAME).hex:i 

clean:
		rm -f $(PROJECT_NAME).S $(PROJECT_NAME).out $(PROJECT_NAME).hex $(FILES)

asm:
		$(AVR_PATH)/avr-gcc -S $(CFLAGS) main.c
