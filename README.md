# Solar Power Management PCB ☀️
The following project is a power management PCB designed to charge a lithium-ion battery using a solar panel and discharge it into an Arduino for low-power applications.

<p align="center">
    <img title="PCB Business Card Render" alt="PCB Business Card" src="./Blender_Render/PCB_Business_Card_Blender.png" width ="75%">
</p>
<p align="center"><i>Blender Render</i></p>

## Arduino Low-Power Application 🔋
Before diving into the PCB design I am going to explore the low-power application that I have designed for this project. The application [Full_Functionality.ino](./Arduino_Code/Full_Functionality/Full_Functionality.ino) utilizes the [DHT11 sensor](https://www.arduino.cc/reference/en/libraries/dht11/) to record temperature and humidity readings, then writes them to the onboard EEPROM (1 KB). Once the data has been stored in the EEPROM the arduino goes into a lower power state by utilizing the [narcoleptic library](https://github.com/brabl2/narcoleptic). I am running this entire application on an [Arduino Uno REV3](https://docs.arduino.cc/hardware/uno-rev3/#tech-specs).

For this application, I would like the Arduino to last as long as possible on battery power. To reduce it's power consumption I referenced the following two articles: [The Arduino Guide to Low Power Design](https://docs.arduino.cc/learn/electronics/low-power/), [Arduino minimal power usage](https://arduino.stackexchange.com/questions/21616/arduino-minimal-power-usage), and [Power saving techniques for microprocessors
](https://www.gammon.com.au/power). A lot of the common techniques involved disabling unused peripherals, reducing clock speeds, and forcing the CPU into a low power mode. One extremely effective method that <b>I did not</b> attempt was utilizing a "bare bones" Atmega328 board. By stripping away a lot of unnecessary components (i.e. USB controller, LDO, etc...) you can significantly reduce the power consumption.

## Arduino Pin-out

| **Arduino** | **DHT11** |
| ------------- | ------------- |
| 5V            | VCC           | 
| GND           | GND           |   
| 2/PD2         | DATA          | 

<br>

<p align="center">
    <img title="Arduino Pin-out" alt="Arduino Pin-out" src="./Images/DHT11-Schematic.png" width ="75%">
</p>
<p align="center"><i>Arduino Pin-out</i></p>

## Power Consumption Tests 🪫
To test the power consumption of the Arduino, I used a bench power supply, a 100-ohm shunt resistor, and the Analog Discovery 2 USB oscilloscope.

### Default Sketch
For the first test I had the Arduino running the default sketch as seen below.
```C++
void setup () {}
void loop () {}
```
The current consumption was measured at two supply voltage levels: 12V and 8V. Realistically, for our application, we will be running the Arduino off 8V through the barrel connector.
* 12V - 44.17mA
* 8V - 25.07mA
<p align="center">
    <img title="Arduino Current Draw 12V" alt="Arduino Current Draw 12V" src="./Images/Arduino_Current_Draw_12V.png" width ="49%">
    <img title="Arduino Current Draw 8V" alt="Arduino Current Draw 8V" src="./Images/Arduino_Current_Draw_8V.png" width ="49%">
</p>
<p align="center"><i>Current Draw 12V [LEFT] vs 8V [RIGHT]</i></p>

### Low_Power_Sketch.ino
The Low_Power_Sketch.ino utilizes the sleep, wdt, and power library to lower the Arduino's power consumption. To lower the Arduino's power consumption I did the following:

* Desoldered the 5V 'ON' LED
* Disabled USER LED 13
* Disabled ADC
* Changing the clock pre-scalar had no effect on the power consumption so I left it as 1.
* Disabled all peripheral modules (SERIAL, ADC, SPI, USART, TIMER 0, TIMER 1, TIMER 2, I2C)
* Disabled brown-out feature. Brownout detection is a hardware feature that shuts down the processor if the system voltage is below a threshold
* Put the CPU into [SLEEP_MODE_PWR_DOWN](https://onlinedocs.microchip.com/pr/GUID-A834D554-5741-41A3-B5E1-35ED7CD8250A-en-US-5/index.html?GUID-825A28EE-C4E4-4C03-864F-92AA9BA41231). The Microchip web page explains what occurs during this mode.

The resulting current consumptions was:
* 8V - 18.78mA

<p align="center">
    <img title="Arduino Low Power Sketch" alt="Arduino Low Power Sketch" src="./Images/Arduino_Low_Power_Sketch.png" width ="100%">
</p>
<p align="center"><i>Current Draw Low_Power_Sketch.ino</i></p>

### Narcoleptic_Sketch.ino
The Narcoleptic_Sketch.ino uses many of the same techniques as the above sketch; however, this script utilizes the [Narcoleptic library](https://github.com/brabl2/narcoleptic) to achieve its low power consumption. The following techniques were used:

* Desoldered the 5V 'ON' LED
* Disabled USER LED 13
* Disabled ADC
* Changing the clock pre-scalar had no effect on the power consumption so I left it as 1.
* Disabled all peripheral modules (SERIAL, ADC, SPI, USART, TIMER 0, TIMER 1, TIMER 2, I2C)
* Disabled brown-out feature. Brownout detection is a hardware feature that shuts down the processor if the system voltage is below a threshold
* Narcoleptic.Delay(x_milliseconds) is called to force the CPU in a low-power state for x_milliseconds.

The resulting current consumptions was:
* 8V - 18.27mA

<p align="center">
    <img title="Arduino Narcoleptic Sketch" alt="Arduino Narcoleptic Sketch" src="./Images/Arduino_Narcoleptic_Sketch.png" width ="100%">
</p>
<p align="center"><i>Current Draw Narcoleptic_Sketch.ino</i></p>

### Full_Functionality.ino
The Full_Functionality.ino sketch is the main program that performs the tasks described in the introduction while using the low power modes that were used in Narcoleptic_Sketch.ino. 

The resulting current consumptions was:
* Awake mode (8V) - 22.42mA
* Low power mode (8V) - 18.30mA

<p align="center">
    <img title="Arduino Full_Functionality Sketch" alt="Arduino Full_Functionality Sketch" src="./Images/Arduino_Full_Functionality_Sketch.png" width ="100%">
</p>
<p align="center"><i>Full_Functionality.ino Low power mode [LEFT] vs Awake mode [RIGHT]</i></p>

### Future Power Consumption Improvements
Despite the power savings documented above, there are still quite a few methods that were left out during this process, which can yield large power savings.

* The first method is extensively discussed in the [Power saving techniques for microprocessors](https://www.gammon.com.au/power) article. You can strip various unnecessary parts from the Arduino, leaving you with a bare-bones version of the board. This includes the voltage regulators (for +5V and +3.3V), USB interface chip (for the USB port), and the "power" LED. According to the article, this resulted in a current consumption of 360uA!
    * The Arduino Uno Rev 3 uses the NCP1117ST50T3G as it's 5V linear voltage regulator. This regulator has a quiescent current of 10mA! If you bypass the LDO you can avoid wasting power.
* Secondly, the ATmega328P MCU can be run at lower frequencies and hence a lower voltage which can reduce the boards power consumption

## Power Management Components ⚡
Now that we know the power consumption of the Arduino, we can begin picking components for this project. This section will focus on the choice of solar panel and lithium-ion battery.

### Lithium Ion Battery 🔋
For the battery, I ended up selecting a pair of lithium-ion batteries I already had at home. The batteries were two 3.7V 2600mAh lithium-ion batteries from PKCELL. [Li-ion Battery ICR18650 2600mAh 3.7V Datasheet](https://www.parts-express.com/pedocs/specs/142-202--pkcell-flat-top-18650-li-ion-3.7v-2600mah-rechargeable-battery-spec-sheet.pdf). Some important specifications to note about this battery are the following:
* Nominal Voltage - 3.7V
* Charging Cut-off Voltage - 4.2V
* Discharge Cut-off Voltage - 3.0V
* Standard Charge - Constant Current 0.5C Constant Voltage 4.2V 0.01 C cut-off
* Standard Discharge - Discharge at 0.2 C to 3.0V


<p align="center">
    <img title="Lithium Ion Batteries" alt="Lithium Ion Batteries" src="./Images/PKCELL-18650-3.7V-2600mAh-Rechargeable-Lithium-Battery-11.png" width ="50%">
</p>
<p align="center"><i>PKCELL Lithium Ion Batteries</i></p>

#### Arduino Low-Power Application Discharge Rate
Now that we know the battery capacity and the Arduino's current consumption, we can determine the lifespan of the battery while supplying the Arduino.

| **Variable**  | **Description**           | **Value** |
| ------------- | -------------             | --------- |
| C             | Battery Capacity          | 5200 mAh  | 
| Is            | Current while sleeping    | 18.3mA    |
| Ia            | Current while awake       | 22.4mA    |
| Wph           | # of wakeups per hour     | 1         |
| Wt            | Duration of a single wake | 1s        |

Time awake per hour: ${Twph\:=\:Wph\cdot Wt\:=\:1s}$

Time asleep per hour: ${Tsph\:=\:msph-Wtph\:=\:3599s}$

Average current draw: ${Iavg\:=\:\left(\left(Ia\cdot Twph\right)+\left(Is\cdot Tsph\right)\right)\:/\:3600s\:=\:18.3011mA}$

Life of battery: ${\frac{C}{I_{avg}}=\frac{5200mAh}{18.3011mA}=284.136\:h=11.83\:days}$

Alternatively, you could use this [Battery Life Calculator](https://oregonembedded.com/batterycalc.htm), which includes the self-discharge rate of a battery in its calculations. Based on this calculator, the battery's life ends up being 10.06 days.

### Solar Panel 🔆
To increase the lifespan of the battery, you could further decrease the current consumption of the Arduino using the previously mentioned methods, or you could hook up a solar panel to recharge the batteries.

After a bit of searching I was able to find a 12V 4.2W (350mA Max) portable [solar panel module](https://www.amazon.ca/Portable-Polysilicon-Battery-Charger-Efficiency/dp/B07S1P67HL/ref=sr_1_97_sspa?crid=2WAO18NW0X2GN&dib=eyJ2IjoiMSJ9.fZVIoLELaw3YZhhkogK5VGuvzYc6wwH_Px50gEwUL5AK6Nxr_yeX3NtH-g3Q1hlIK30yAnZ0evMQHhDVoLYAm4sbS6SBN7zU1qKFeF3U7BU.zWxxjvWhspQamoYMqpsdiUn7VfXNYG4ixH8fwb_Tuoc&dib_tag=se&keywords=solar+cell&qid=1717475335&s=hi&sprefix=solar+ceil%2Ctools%2C96&sr=1-97-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGZfbmV4dA&psc=1).

#### Ideal Conditions
If we assume ideal conditions where the sun is visible 100% of the day we can determine how long it would take for the solar panel to recharge the battery. 
* ${t}$ is the time (in hours) it takes to charge the battery
* ${Q}$ is the capacity of the battery (in mAh), which is 5200mAh in our case
* ${I}$ is the current (in mA) that the panel can supply, which in our case is 350mA max.

${t=\frac{Q}{I}=\frac{5200}{350}=14.85\:hours}$

#### Real World Conditions
For real-world conditions, we can assume that the total hourly power is closer to 50% of the solar panel's maximum power (175mA). Additionally, we can include the power consumption of the Arduino, which is around 18.3mA.
* ${t}$ is the time (in hours) it takes to charge the battery
* ${Q}$ is the capacity of the battery (in mAh), which is 5200mAh in our case
* ${Iload}$ is the net current (in mA) that the Arduino is drawing, which is 18.3mA in our case
* ${Isolar}$ is the net current (in mA) that the solar panel is providing, which is 175mA in our case. 

${I_{net}=I_{solar}-I_{net}=175mA-18.3mA=156.7mA}$

${t=\frac{Q}{I_{net}}=\frac{5200}{156.7}=33.18\:hours}$

## Battery Management IC 📶
THe NFC IC being used in this project is the [NT3H1101W0FTTJ](https://www.digikey.ca/en/products/detail/nxp-usa-inc/NT3H1101W0FTTJ/5347877).
* This chip is an NFC chip designed by NXP. It features I2C communication, energy harvesting, and field detection features. 
* It features 64 bytes of SRAM and 1kB of EEPROM memory. We can use the onboard memory to stored a personal website link for the user device to retrieve when communicating with the IC.
* Lastly with this chip, we can utilize the induced current to power the chip (i.e., energy harvesting).
* We can program the IC through the [NFC TagWriter by NXP](https://play.google.com/store/apps/details?id=com.nxp.nfc.tagwriter&hl=en_GB) app.

<p align="center">
    <img title="NFC IC " alt="NFC IC" src="./Images/NFC_IC_Image.png" width ="40%">
</p>

## NFC Equivalent Circuit
The internet has a plethora of resources for antenna design. The two sources I will be referring to for this design are:
* [AN11276 - NXP Community](https://community.nxp.com/pwmxy87654/attachments/pwmxy87654/nfc/6155/1/AN11276.pdf)
* [AN2972 Application note How to design an antenna for dynamic NFC tags](https://www.st.com/resource/en/application_note/an2972-how-to-design-an-antenna-for-dynamic-nfc-tags-stmicroelectronics.pdf)

The figure below shows the equivalent electrical circuits of the dynamic NFC tag and its antenna.

<p align="center">
    <img title="NFC Equivalent Circuit" alt="NFC Equivalent Circuit" src="./Images/Equivalent_Circuit.png" width ="75%">
</p>

The above circuit can be simplified into an equivalent RLC circuit.

<p align="center">
    <img title="NFC RLC Circuit" alt="NFC RLC Circuit" src="./Images/Simplified_Equivalent_Circuit.png" width ="75%">
</p>

### Resonance Frequency
Based on the simplified RLC circuit we can determine the resonant frequency. 
* Resonance frequency is the frequency (in hertz) in which a signal or power level can be carried, or couple from one device to another with no power or signal loss "ideally".
* $f_R=\frac{1}{2\cdot \pi \cdot \sqrt{L_{pc}\cdot C_{pl}}}$
    * $C_{pl}=C_{tun}+C_{ant}$

We know that for an NFC tag, the desired frequency is 13.56 MHz. We also know that the IC's parallel capacitance is 50 pF (from the datasheet). <i>Ignore the parasitic capacitance of the antenna for now; we will come back to that</i>. Based on this information, we can work backwards to determine the required inductance of the antenna.

$f_R=\frac{1}{2\cdot \pi \cdot \sqrt{L_{pc}\cdot C_{pl}}}$

$L_{pc}=\frac{1}{C_{pl}\left(2\pi f_R\right)^2}$

$L_{pc}=\frac{1}{5\cdot 10^{-11}\left(2\pi \cdot 13560000\right)^2} = 0.00000275518$

$L_{pc}=0.00000275518\:H=\:2.75518\:uH$

## Antenna Design 📡
We can now design an NFC antenna based on the desired inductance calculated above. To design the antenna I referred to various resources to confirm my calculations. 

### NXP Spiral Antenna Calculation
The [AN11276 NTAG Antenna Design Guide](https://community.nxp.com/pwmxy87654/attachments/pwmxy87654/nfc/6155/1/AN11276.pdf) provides the following information to design an antenna. 

<p align="center">
    <img title="NXP Spiral Formula" alt="NXP Spiral Formula" src="./Images/NXP_Class_4_Antenna.png" width ="75%">
</p>
<p align="center"><i>NXP Class 4 Antenna Design Recommendation</i></p>

<p align="center">
    <img title="NXP Spiral Formula" alt="NXP Spiral Formula" src="./Images/NXP_Spiral_Antenna_Formula.png" width ="75%">
</p>
<p align="center"><i>NXP Round Antennas Formula</i></p>

I utilized the [Antenna_Shape_Calculator](/Resource_Documents/Antenna_Shape_Calculator.xlsx) excel document to calculate the geometric parameters. They were as follows:
* $D_0 = 41mm$
* $t = 35um$
* $w = 300um$
* $g = 300um$
* $Nant = 6\:turns$
* $L_{calc} = 2.754uH$

### STM Spiral Antenna Calculation
The [AN2972 Application note](https://www.st.com/resource/en/application_note/an2972-how-to-design-an-antenna-for-dynamic-nfc-tags-stmicroelectronics.pdf) provides the following information to design an antenna. 

<p align="center">
    <img title="STM Spiral Formula" alt="STM Spiral Formula" src="./Images/STM_Antenna.png" width ="75%">
</p>
<p align="center"><i>STM Spiral Antenna Design</i></p>

<p align="center">
    <img title="STM Spiral Formula" alt="STM Spiral Formula" src="./Images/STM_Spiral_Antenna_Formula.png" width ="75%">
</p>
<p align="center"><i>STM Spiral Antennas Formula</i></p>

I utilized the [Antenna_Shape_Calculator](/Resource_Documents/Antenna_Shape_Calculator.xlsx) excel document to calculate the geometric parameters. They were as follows:
* $r_{in} = 17.5mm$
* $r_{out} = 20.5mm$
* $Nant = 6\:turns$
* $L_{calc} = 2.766uH$

### Missing Parameters 
We are not quite ready to complete the design. Looking back at the equivalent circuit, we are still missing a few parameters: antenna capacitance, antenna resistance, and the NFC IC resistance. Typically, these parameters are determined by measuring the actual physical circuit design. For a guide on how to measure these parameters, check out the following link: --> [Guide to designing antennas for the NTAG I2C plus](https://community.nxp.com/t5/NXP-Designs-Knowledge-Base/Guide-to-designing-antennas-for-the-NTAG-I2C-plus/ta-p/1104729).  Nevertheless, we can make some rough estimations by utilizing various tools.

* Antenna resistance - By using a parasitic resistance tool in KiCad, we can calculate this parameter. According to the tool, the parasitic resistance was 1.17 ohms. 
* Antenna capacitance - By utilizing the [NXP NFC Antenna Tool](https://community.nxp.com/t5/NFC/bd-p/nfc/page/4?_gl=1*gmz3or*_ga*NzkyMDk1NzYwLjE3MTEwNTg3NTE.*_ga_WM5LE0KMSH*MTcxMTU1MzAwMS4xMy4wLjE3MTE1NTMwMDEuMC4wLjA.) we can roughly estimate this parameter. This tool does not support spiral designs, but we can use the rectangular design to estimate this parameter. According to the tool, the parasitic capacitance was 1.9 pF.
* NFC IC Resistance - Assuming the voltage should drop entirely across the IC, we can consider this resistance to be infinitely large. For our purposes, we will estimate it to be 100k ohms.


<p align="center">
    <img title="NXP NFC Antenna Tool" alt="NXP NFC Antenna Tool" src="./Images/NXP_Antenna_Tool.png" width ="75%">
</p>
<p align="center"><i>NXP NFC Antenna Tool</i></p>

## Simulation
We can now work our way backwards to verify the parameters we calculated above. By rebuilding the equivalent circuit with these parameters in LTSpice, we can verify the resonance frequency. According to the simulation below, the resonance frequency is 13.35 MHz. This should be adequate for NFC; alternatively, we can target an inductance of 2.5-2.6 uH to achieve a resonance frequency that is closer to 13.56 MHz. Furthermore, an optional tuning capacitor can be added to get closer to 13.56 MHz.

<p align="center">
    <img title="LTSpice Simulation" alt="LTSpice Simulation" src="./Images/LTSpice_Simulation.png" width ="75%">
</p>
<p align="center"><i>LTSpice Simulation</i></p>

I decided to rerun the simulation and redesign the antenna parameters ($D_0 = 39mm$) to target an inductance of 2.576uH. This gives us more headroom to account for any imperfections in the manufacturing process. This allows us to use a tuning capacitor to bring the resonance frequency down and closer to 13.56 MHz.  

<p align="center">
    <img title="LTSpice Simulation Tuned" alt="LTSpice Simulation Tuned" src="./Images/LTSpice_Simulation_Tuned.png" width ="75%">
</p>
<p align="center"><i>LTSpice Simulation with Tuning Capacitor</i></p>

### Electromagnetic Simulation 🧲
Antennas are typically simulated in electromagnetic simulation software. There are various tools available, such as:
* Ansys HFSS - The student edition does not support circuit simulation nor does it support import/export
* Solidworks CST Microwave -  The student edition does not support circuit simulation nor does it support import/export 
* openEMS
* MATLAB Antenna Toolbox

For this design, I ended up simulating it in MATLAB because I did not have access to the non-student versions of HFSS or CST.

#### MATLAB Antenna Toolbox
By utilizing [MATLAB's Antenna Toolbox](https://www.mathworks.com/help/antenna/), we can generate a spiral antenna as well as its S-parameters based on the network-matched design.

<p align="center">
    <img title="MATLAB Antenna Simulation" alt="MATLAB Antenna Simulation" src="./Antenna_Simulations/MATLAB_Antenna_Simulation/MATLAB_Antenna_Spiral_Image.png" width ="75%">
</p>
<p align="center"><i>MATLAB Archimedean Spiral</i></p>

<p align="center">
    <img title="MATLAB Antenna Simulation" alt="MATLAB Antenna Simulation" src="./Antenna_Simulations/MATLAB_Antenna_Simulation/MATLAB_3D_Radiation_Image.png" width ="75%">
</p>
<p align="center"><i>MATLAB 3D Radiation Pattern</i></p>

<p align="center">
    <img title="MATLAB Antenna Simulation" alt="MATLAB Antenna Simulation" src="./Antenna_Simulations/MATLAB_Antenna_Simulation/Matching_Network_Sparameters_Image.png" width ="75%">
</p>
<p align="center"><i>MATLAB Matching Network S-parameters</i></p>

S11 represents the return loss of a device, indicating how much of the input power supplied to the device reflects back to the input port. For an NFC antenna design, it is desired for it to have the lowest gain at at 13.56 MHz. 

## PCB Design
For the final design of the antenna I ended up settling on the following parameters:
* $D_0 = 39mm$
* $t = 35um$
* $w = 300um$
* $g = 300um$
* $Nant = 6\:turns$
* $L_{calc} = 2.576uH$
* $R_{ant} = 1.17\:ohms$
* $C_{ant} = 2pF$
* $Simulated\:resonance\:frequency:\:13.75 MHz$ 
    * *Without Tuning Capacitor*

For the schematic, I borrowed the idea of an LED indicator for the energy harvesting circuit from the [PCB Business Card With NFC](https://www.instructables.com/PCB-Business-Card-With-NFC/). The capacitor is to guarantee operation during RF communication. The resistor value was chosen to limit the current going through the LED. The value was determined by using the following equation:
* $R\:=\:\frac{V_{CC}-V_{LED}}{I_{LED}}=\frac{3.3-2}{0.02} = 65Ω$

<p align="center">
    <img title="KiCad Schematic Design" alt="KiCad Schematic Design" src="./Images/Schematic_Image.png" width ="75%">
</p>
<p align="center"><i>Schematic Design</i></p>
<p align="center">
    <img title="KiCad PCB Design" alt="KiCad PCB Design" src="./Images/PCB_Image.png" width ="75%">
</p>
<p align="center"><i>PCB Design</i></p>

## Final BOM 📃
* [NT3H1101W0FTTJ - IC RFID TRANSP 13.56MHZ 8TSSOP](https://www.digikey.ca/en/products/detail/nxp-usa-inc/NT3H1101W0FTTJ/5347877)
* [150060VS75000 - LED GREEN CLEAR 0603 SMD](https://www.digikey.ca/en/products/detail/w%C3%BCrth-elektronik/150060VS75000/4489906)
* [C0603C224J4REC7867 - CAP CER 0603 220NF 16V X7R 5%](https://www.digikey.ca/en/products/detail/kemet/C0603C224J4REC7867/8643651)
* [CRCW060364R9FKEA - RES SMD 64.9 OHM 1% 1/10W 0603](https://www.digikey.ca/en/products/detail/vishay-dale/CRCW060364R9FKEA/1174533)
* [GRM0335C1E1R5BA01D - CAP CER 1.5PF 25V C0G/NP0 0201 (0603 Metric)](https://www.digikey.ca/en/products/detail/murata-electronics/GRM0335C1E1R5BA01D/4358584)

## Manufacturing 🪛
I ordered the board through PCBWay by using the [KiCAD plugin](https://www.pcbway.com/blog/News/PCBWay_Plug_In_for_KiCad_3ea6219c.html). I assembled and soldered the components on the board myself. 

For the PCBWay specification selection I chose the following options:
* Layers: 2
* Material: FR-4
* FR4-TG: TG130-140
* Thickness: 1.6mm
* Min track/spacing: 5/5mil
* Solder mask: Green
* Silkscreen: White
* Edge connector: No
* Surface finish: HASL with lead
* Via process: Tenting vias
* Finished copper: 1 oz Cu
* Remove product Num: No

<p align="center">
    <img title="PCBWay PCB" alt="PCBWay PCB" src="./Images/PCB_Business_Card_PCBWay.jpg" width ="75%">
</p>
<p align="center"><i>Final Product</i></p>

## Programming 💻
Lastly, I programed the IC through the [NFC TagWriter by NXP](https://play.google.com/store/apps/details?id=com.nxp.nfc.tagwriter&hl=en_GB) app.

<p align="center">
    <img title="PCB GIF" alt="PCB GIF" src="./Images/PCB_GIF.gif" width ="75%">
</p>
<p align="center"><i>PCB in Action!</i></p>

## Characterization 🔬
Finally, I wanted to characterize the response of the circuit using my Analog Discovery 2 - USB oscilloscope. In particular, I aimed to characterize and visualize the difference in responses when the circuit had and didn't have a tuning capacitor.

### No Tuning Capacitor

The first order of business was to utilize the network analyzer feature on the Analog Discovery 2. By attaching the wave generator on one end of the antenna and probing on the other we could sweep through various frequencies and observe the circuits response.

<p align="center">
    <img title="Network Analyzer" alt="Network Analyzer" src="./Images/Network_Analysis.png" width ="100%">
</p>
<p align="center"><i>Network Analyzer</i></p>

The above graph describes the S11 parameter, which represents the return loss of a device, indicating how much of the input power supplied to the device reflects back to the input port. We observe that at 14 MHz, the graph dips to a gain of -44.9 dB. This indicates that at 14 MHz, the magnitude of the reflected signal is lower than the magnitude of the incident signal. This implies that the antenna is absorbing the incident energy rather than reflecting it back towards the source. This response closely matches what we simulated previously in LTspice and MATLAB. For an NFC antenna, this behavior is desirable at 13.56 MHz. We can generate a graph with a dip closer to 13.56 MHz by including the tuning capacitor!


<p align="center">
    <img title="Spectrum Analyzer" alt="Spectrum Analyzer" src="./Images/Spectrum_Analysis.png" width ="100%">
</p>
<p align="center"><i>Spectrum Analyzer</i></p>

Lastly, during the operation of the circuit, we can utilize the spectrum analyzer to observe the magnitude of the incoming signal. Keep in mind that this circuit doesn't contain a tuning capacitor. This was achieved by probing both ends of the antenna. Once a phone was brought within operating distance, we could see that the antenna contained a signal with a magnitude of 12.37 dB at 13.56 MHz.

### Tuning Capacitor Included (3.9pF)

By introducing a tuning capacitor with a value of 3.9pF, I was able to adjust the circuit's response to achieve resonance closer to the NFC standard of 13.56 MHz. Below is the graph produced by the network analyzer. We can see that this time the graph dips at a frequency of 13.555 MHz with a gain of -44.34 dB.

<p align="center">
    <img title="Network Analyzer Tuned" alt="Network Analyzer Tuned" src="./Images/Network_Analysis_Tuned.png" width ="100%">
</p>
<p align="center"><i>Network Analyzer</i></p>

<p align="center">
    <img title="Spectrum Analyzer Tuned" alt="Spectrum Analyzer Tuned" src="./Images/Spectrum_Analysis_Tuned.png" width ="100%">
</p>
<p align="center"><i>Spectrum Analyzer</i></p>

Utilizing the same process as the previous spectrum analysis, we can observe that the antenna contained a signal with a magnitude of 12.494 dB at 13.562 MHz.

### Poorly Tuned NFC Antenna
By introducing a really large tuning capacitor with a value of 15pF, we can adjust the circuit's response to achieve resonance at 12.5 MHz. Given the poorly tuned antenna, we expect to either see that the NFC capabilities cease to work or that the magnitude of the NFC signal is extremely weak. We can quantify this by utilizing the same spectrum analysis technique as used in the previous examples.

<p align="center">
    <img title="Spectrum Analyzer Poorly Tuned" alt="Spectrum Analyzer Poorly Tuned" src="./Images/Poorly_Tuned_NFC.png" width ="100%">
</p>
<p align="center"><i>Spectrum Analyzer of Poorly Tuned Antenna</i></p>

With the poorly tuned antenna, the NFC capabilities continued to work. However, it did require the phone to be slightly closer to the antenna. Additionally, based on the above graph, we can see that the magnitude of the NFC signal (13.56MHz) is significantly weaker than in previous examples.

## NFC Protocol
By utilizing the Analog Discovery 2 oscilloscope feature, I was able to capture a snippet of an NFC transmission. This was done by probing the antenna during NFC operation. The NFC protocol is extremely complicated, and therefore, it is difficult to decipher what this transmission is. You can click on the following link to learn more about the [NFC transmission protocol](https://scdn.rohde-schwarz.com/ur/pws/dl_downloads/dl_application/application_notes/1ma182/1MA182_5E_NFC_WHITE_PAPER.pdf).

<p align="center">
    <img title="NFC Transmission" alt="NFC Transmission" src="./Images/NFC_Transmission.png" width ="100%">
</p>
<p align="center"><i>NFC Transmission</i></p>

# Resources
* [Guide to designing antennas for the NTAG I2C plus
](https://community.nxp.com/t5/NXP-Designs-Knowledge-Base/Guide-to-designing-antennas-for-the-NTAG-I2C-plus/ta-p/1104729#toc-hId--56223682)
* [PN7160 antenna design and matching guide](https://www.nxp.com/docs/en/application-note/AN13219.pdf)
* https://github.com/shawnalexsony/Business_Card_PCB/tree/main
* [PCBWAY Custom Design Rules Templates](https://www.pcbway.com/pcb_prototype/PCB_Design_Rule_Check.html?currency=1)