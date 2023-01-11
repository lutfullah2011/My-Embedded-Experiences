(000_MY_DRIVERS_000)
**************************************************
* This file include my drivers for the Device & GPIO & SPI 
* I developed these files while taking the Udemy Courses(Mastering Microcontroller and Embedded Driver Development)
**************************************************


(000_Stm32f1xx_Drivers_Examples)
**************************************************
* This file includes 7 examples for now.("Led_toggle_API" & "Led_button" & "Button_interrupt" & "SPI_TX" & "SPI_Tx_StmMaster_ArduinoSlave" & "SPI_TxRx_STM_ARDIUNO_CMD_INTERACTS" & "SPI_Rx_message_from_Ardiuno_InterruptMode" exercises)
* I used my own drivers for these examples.
**************************************************


(ADC - 3CH - DMA - USART)
**************************************************
*CUBEIDE de Cubemx de DMA kullanarak USARTTAN ADC1 deki 3 channeli pot, ldr ve internal temperature değerlerini okuma 
uygulaması yaptım.(24.10.2022)

/*  Aslında daha önce Keil IDE'de bu uygulamayı yapmıştım. Sadece CubeIDE ye entegre etmeye çalıştım ama bir türlü yapamadım.
CubeIDE de F1 kartımı debug edemediğim için sıkıntının nereden kaynaklandığını anlayamadım. Birçok araştırma yapıp farklı
denemeler yaptım. ADC de continuous conversion yaptığımda uygulama duruyor ve ilerlemiyordu. Sürekli çevirim yapmadığımda 
ise değerler değişmiyordu. Biraz daha DMA ADC ilişkisine dair araştırma sonucu DMA adc değerlerini hafızasına attıktan hemen 
sonra HAL_ADC_ConvCpltCallback fonsiyonuna sıçradığını ve burada tıkandığını farkettim. Sonra Flag kullanarak bu sorundan 
kurtuldum. HAL_ADC_Start_DMA komutunda uygulamamı orada beklettim ve hafızaya atması için while içinde beklettim. Sonra 
HAL_ADC_ConvCpltCallback içinde ise bayrağı 1  yapıyordum.   */
**************************************************


(I2C - OLED - ADC - DMA - APPLICATION)
**************************************************
*CUBEIDE de Cubemx kullanarak I2C kullanarak ADC ile okuduğum pot,ldr ve internal temp değerlerini OLED ekrana yansıtma uygulaması yaptım.(25.10.2022)
**************************************************


(I2C - EEPROM - OLED - APPLICATION)
**************************************************
*I2C daha iyi anlamak ve önceki bilgilerimi genişletmek için harici EEPROM ile OLED ekran kullanarak önceki projemi genişlettim.(01.11.2022)
* EEPROM driverını internet ortamından bulup kullanmaya uygun hale getirdim.
**************************************************


(Internal_Flash_Memory)
**************************************************
*CUBEIDE de Cubemx kullanarak Internal Flash Memory uygulaması yaptım. Basitçe ROM hafızaya veri yazıp okuma uygulaması.(29.10.2022)
**************************************************


(LOW_LEVEL - Button_Interrupt)
**************************************************
*CUBEIDE de Cubemx KULLANMADAN register seviyesinde, pointer yardımıyla adresleme, bitwise operatörleri kullanarak interrupt 
uygulaması geliştirmeye çalıştım ama NVIC adresini Memory Mapden bulamadığım(Çözüm arayışım yetersiz kaldı belki de) 
için uygulamam yarım kaldı:((
Update: Evet, adresi buldum:))) ARM işlemci olduğu için onun dökümanında adres var. Interrrupt kavramını daha iyi anlamış oldum ve uygulama çalışıyor(08.12.2022) 
**************************************************


(LOW_LEVEL - Clock_Configuration_ADC1)
**************************************************
*CUBEIDE de Cubemx KULLANMADAN register seviyesinde ADC1 için base adresler, SFR(Special Function Register)ın ve RCC dahil 
 basitçe bir uygulama gerçekleştirdim.
**************************************************


(LOW_LEVEL - Led_On)
**************************************************
*CUBEIDE de Cubemx KULLANMADAN register seviyesinde Led yakma uygulaması gerçekleştirdim.
**************************************************


