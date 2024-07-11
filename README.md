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
    <img title="Arduino Pin-out" alt="Arduino Pin-out" src="./Images/DHT11-Schematic.png" width ="50%">
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
For the battery, I ended up selecting a pair of lithium-ion batteries I already had at home. The batteries were two 3.7V 2600mAh lithium-ion batteries from PKCELL. The batteries were arranged in series in a battery pack to produce an output voltage of 7.4V and a capacity of 2600 mAh. [Li-ion Battery ICR18650 2600mAh 3.7V Datasheet](https://www.parts-express.com/pedocs/specs/142-202--pkcell-flat-top-18650-li-ion-3.7v-2600mah-rechargeable-battery-spec-sheet.pdf). Some important specifications to note about this battery are the following:
* Nominal Voltage - 3.7V
* Charging Cut-off Voltage - 4.2V
* Discharge Cut-off Voltage - 3.0V
* Standard Charge - Constant Current 0.5C Constant Voltage 4.2V 0.01 C cut-off
* Standard Discharge - Discharge at 0.2 C to 3.0V


<p align="center">
    <img title="Lithium Ion Batteries" alt="Lithium Ion Batteries" src="./Images/PKCELL-18650-3.7V-2600mAh-Rechargeable-Lithium-Battery-11.png" width ="25%">
</p>
<p align="center"><i>PKCELL Lithium Ion Batteries</i></p>

#### Arduino Low-Power Application Discharge Rate
Now that we know the battery capacity and the Arduino's current consumption, we can determine the lifespan of the battery while supplying the Arduino.

| **Variable**  | **Description**           | **Value** |
| ------------- | -------------             | --------- |
| C             | Battery Capacity          | 2600 mAh  | 
| Is            | Current while sleeping    | 18.3mA    |
| Ia            | Current while awake       | 22.4mA    |
| Wph           | # of wakeups per hour     | 1         |
| Wt            | Duration of a single wake | 1s        |

Time awake per hour: ${Twph=Wph\cdot Wt=1s}$

Time asleep per hour: ${Tsph=msph-Wtph=3599s}$

Average current draw: ${Iavg=\left(\left(Ia\cdot Twph\right)+\left(Is\cdot Tsph\right)\right)/3600s=18.3011\thinspace mA}$

Life of battery: ${\frac{C}{I_{avg}}=\frac{2600mAh}{18.3011mA}=142.067\thinspace hours=5.919\thinspace days}$

Alternatively, you could use this [Battery Life Calculator](https://oregonembedded.com/batterycalc.htm), which includes the self-discharge rate of a battery in its calculations. Based on this calculator, the battery's life ends up being 5.03 days.

### Solar Panel 🔆
To increase the lifespan of the battery, you could further decrease the current consumption of the Arduino using the previously mentioned methods, or you could hook up a solar panel to recharge the batteries.

After a bit of searching I was able to find a 12V 4.2W (350mA Max) portable [solar panel module](https://www.amazon.ca/Portable-Polysilicon-Battery-Charger-Efficiency/dp/B07S1P67HL/ref=sr_1_97_sspa?crid=2WAO18NW0X2GN&dib=eyJ2IjoiMSJ9.fZVIoLELaw3YZhhkogK5VGuvzYc6wwH_Px50gEwUL5AK6Nxr_yeX3NtH-g3Q1hlIK30yAnZ0evMQHhDVoLYAm4sbS6SBN7zU1qKFeF3U7BU.zWxxjvWhspQamoYMqpsdiUn7VfXNYG4ixH8fwb_Tuoc&dib_tag=se&keywords=solar+cell&qid=1717475335&s=hi&sprefix=solar+ceil%2Ctools%2C96&sr=1-97-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGZfbmV4dA&psc=1).

#### Ideal Conditions
If we assume ideal conditions where the sun is visible 100% of the day we can determine how long it would take for the solar panel to recharge the battery. 
* ${t}$ is the time (in hours) it takes to charge the battery
* ${Q}$ is the capacity of the battery (in mAh), which is 2600mAh in our case
* ${I}$ is the current (in mA) that the panel can supply, which in our case is 350mA max.

${t=\frac{Q}{I}=\frac{2600}{350}=7.42\thinspace hours}$

#### Real World Conditions
For real-world conditions, we can assume that the total hourly power is closer to 50% of the solar panel's maximum power (175mA). Additionally, we can include the power consumption of the Arduino, which is around 18.3mA.
* ${t}$ is the time (in hours) it takes to charge the battery
* ${Q}$ is the capacity of the battery (in mAh), which is 2600mAh in our case
* ${Iload}$ is the net current (in mA) that the Arduino is drawing, which is 18.3mA in our case
* ${Isolar}$ is the net current (in mA) that the solar panel is providing, which is 175mA in our case. 

${I_{net}=I_{solar}-I_{net}=175\thinspace mA-18.3\thinspace mA=156.7\thinspace mA}$

${t=\frac{Q}{I_{net}}=\frac{2600}{156.7}=16.6\thinspace hours}$

## Battery Management IC 🔌
The battery management IC being used in this project is the [LT3652](https://www.analog.com/en/products/lt3652.html)

* The LT3652 is a complete monolithic step-down battery charger designed for solar power and capable of supporting multi-chemistry, multi-cell batteries.
* Input Supply Voltage Regulation Loop for MPPT Solar Applications
* Wide Input Voltage Range: 4.95V to 32V (40V Abs Max)
* Programmable Charge Rate: Up to 2A
* User-Selectable Termination: C/10 or On-Board Termination Timer
* Resistor Programmable Float Voltage: Up to 14.4V
* Accommodates Li-Ion/Polymer, LiFePO4, SLA Chemistries
* The primary reason for choosing this IC was because a simulation model for it is available in LTspice.

<p align="center">
    <img title="Battery Management IC " alt="Battery Management IC" src="./Images/LT3652_IC.png" width ="25%">
</p>

## Design Notes
- [x] VIN Input Supply
- [x] Charge Current Programming
- [x] BOOST Supply
- [x] VIN/BOOST Start-Up Requirement
- [x] VBAT Output Decoupling
- [x] Inductor Selection
- [x] Rectifier Selection
- [x] Battery Float Voltage Programming
- [x] Input Supply Voltage Regulation
- [x] Battery Voltage Temperature Compensation
- [x] Status Pins
- [x] C/10 Termination

### VIN Input Supply
* A high-quality, low ESR decoupling capacitor is recommended to minimize voltage glitches on VIN

#### RMS Ripple Current
${I_{CVIN\left(RMS\right)}≅I_{CHG\left(MAX\right)}•\left(V_{BAT}/V_{IN}\right)•\left(\left[V_{IN}/V_{BAT}\right]-1\right)^{\frac{1}{2}}}$

${I_{CVIN\left(RMS\right)}=0.5\cdot \left(\frac{7.4}{12}\right)\cdot \left(\left(\frac{12}{7.4}\right)-1\right)^{\frac{1}{2}}=0.2431\thinspace A}$

* where ${I_{CHG\left(MAX\right)}}$ is the maximum average charge current
(100mV/${R_{SENSE}}$).
* The simple worst-case of ½ • ${I_{CHG\left(MAX\right)}}$ is commonly
used for design.

#### Input Ripple Voltage
Bulk capacitance is a function of desired input ripple voltage (ΔVIN), and follows the relation:
${C_{IN{\left(BULK\right)}}=I_{CHG{\left(MAX\right)}}•\left(V_{BAT}/V_{IN}\right)/∆V_{IN}\thinspace \left(µF\right)}$

${C_{IN\left(BULK\right)}=0.5\cdot \frac{\left(\frac{7.4}{12}\right)}{0.1}=3.083\thinspace uF}$

* Input ripple voltages above 0.1V are not recommended. 10µF is typically adequate for most charger applications.

### Charge Current Programming
The data sheet describes the charge current programming as follows: <i>"The LT3652 charger is configurable to charge at average currents as high as 2A. Maximum charge current is set by choosing an inductor sense resistor (RSENSE)..."</i> 

${R_{SENSE}\thinspace = \thinspace 0.1/I_{CHG\left(MAX\right)}}$

The maximum charging current is limited by the maximum current output of the solar panel, which is 350 mA. Therefore, we can set the limit to 500 mA to allow for some headroom.

${R_{SENSE}\thinspace = \thinspace 0.1/I_{CHG\left(MAX\right)}=0.1/0.5=0.2Ω}$

### BOOST Supply
The voltage on the decoupling capacitor is refreshed through a diode, with the anode connected to either the battery output voltage or an external source, and the cathode connected to the BOOST pin. Rate the diode average current greater than 0.1A, and reverse voltage greater than VIN(MAX).

### VIN/BOOST Start-Up Requirement
When an LT3652 charger is not switching, the SW pin is
at the same potential as the battery, which can be as high
as VBAT(FLT). As such, for reliable start-up, the VIN supply
must be at least 3.3V above VBAT(FLT). Once switching
begins and the BOOST supply capacitor gets charged
such that (VBOOST – VSW) > 2V, the VIN requirement no
longer applies.

### VBAT Output Decoupling
An LT3652 charger output requires bypass capacitance connected from the BAT pin to ground (CBAT). A 10µF ceramic capacitor is required for all applications.

### Inductor Selection
The primary criterion for inductor value selection in an LT3652 charger is the ripple current created in that inductor.
* Once the inductance value is determined, an inductor must also have a saturation current equal to or exceeding the maximum peak current in the inductor
* ${L=\frac{10•R_{SENSE}}{\frac{ΔI_L}{I_{CHG\left(MAX\right)}}}•V_{BAT\left(FLT\right)}•\left(1-\frac{V_{BAT\left(FLT\right)}}{V_{IN\left(MAX\right)}}\right)\thinspace \left(µH\right)}$
    * ${L=\frac{10\cdot 0.2}{\frac{0.15}{0.5}}\cdot 7.4\cdot \left(1-\frac{7.4}{12}\right)=18.91\thinspace uH}$    
* In the above relation, VIN(MAX) is the maximum operational voltage. Ripple current is typically set within a range of 25% to 35% of ICHG(MAX), so an inductor value can be determined by setting 0.25 < ΔIL/ICHG(MAX) < 0.35
* Magnetics vendors typically specify inductors with maximum RMS and saturation current ratings. Select an inductor that has a saturation current rating at or above ${\left(1+ΔI_{MAX}/2\right)•I_{CHG\left(MAX\right)}}$, and an RMS rating above ${I_{CHG\left(MAX\right)}}$.
* Inductors must also meet a maximum voltsecond product requirement. If this specification is not in the data sheet of an inductor, consult the vendor to make sure the maximum volt-second product is not being exceeded by your design. The minimum required volt-second product is: ${V_{BAT\left(FLT\right)}•\left(\frac{1-V_{BAT\left(FLT\right)}}{V_{IN\left(MAX\right)}}\right)\left(V•\thinspace µS\right)}$

### Rectifier Selection
The rectifier diode from SW to GND, in a LT3652 battery charger provides a current path for the inductor current when the main power switch is disabled. The rectifier is selected based upon forward voltage, reverse voltage, and maximum current. A Schottky diode is required, as low forward voltage yields the lowest power loss and highest efficiency. The rectifier diode must be rated to withstand reverse voltages greater than the maximum VIN voltage.

The minimum average diode current rating (${I_{DIODE\left(MAX\right)}}$)
is calculated with maximum output current (${I_{CHG\left(MAX\right)}}$),
maximum operational VIN, and output at the precondition
threshold (${V_{BAT\left(PRE\right)},\thinspace or\thinspace 0.7•V_{BAT\left(FLT\right)}}$): ${I_{DIODE\left(MAX\right)}>I_{CHG\left(MAX\right)}•\frac{V_{IN\left(MAX\right)}-V_{BAT\left(PRE\right)}}{V_{IN\left(MAX\right)}}\left(A\right)}$

${I_{DIODE\left(MAX\right)}>0.5\cdot \frac{12-5.18}{12}}$

${I_{DIODE\left(MAX\right)}>0.2841\thinspace A}$

### Battery Float Voltage Programming
The output battery float voltage (VBAT(FLT)) is programmed by connecting a resistor divider from the BAT pin to VFB. VBAT(FLT) can be programmed up to 14.4V.

Using a resistor divider with an equivalent input resistance at the VFB pin of 250k compensates for input bias current error. Required resistor values to program desired VBAT(FLT) follow the equations:
* ${R_{FB1}=\left(V_{BAT\left(FLT\right)}•2.5•10^5\right)/3.3\thinspace \left(Ω\right)}$
* ${R_{FB2}=\left(R_{FB1}•\left(2.5•10^5\right)\right)/\left(R_{FB1}-\left(2.5•10^5\right)\right)\thinspace \left(Ω\right)}$ 


The charge function operates to achieve the final float voltage of 3.3V on the VFB pin. The auto-restart feature initiates a new charging cycle when the voltage at the VFB pin falls 2.5% below that float voltage. Because the battery voltage is across the VBAT(FLT) programming resistor divider, this divider will draw a small amount of current from the battery (IRFB) at a rate of: IRFB = 3.3/RFB2

Precision resistors in high values may be hard to obtain, so for some lower VBAT(FLT) applications, it may be desirable to use smaller-value feedback resistors with an additional resistor (RFB3) to achieve the required 250k equivalent resistance. The resulting 3-resistor network, as shown in Figure 5, can ease component selection and/or increase output voltage precision, at the expense of additional current through the feedback divider.

${\frac{R_{FB2}}{R_{FB1}}=\frac{3.3}{\left(V_{BAT\left(FLT\right)}-3.3\right)}}$

${\frac{R_{FB2}}{R_{FB1}}=\frac{3.3}{\left(7.4-3.3\right)}=0.80487}$

Set the divider current (IRFB) = 10uA yields:

${R_{FB2}=\frac{3.3}{10uA}=330k}$

Solving for RFB1:

${R_{FB1}=\frac{330k}{0.80487}=410k}$

The divider equivalent resistance is:

${R_{FB1}||R_{FB2}=182.8k}$

To satisfy the 250k equivalent resistance to the VFB pin: 

${R_{FB3}=250k-182.8k=67.2k}$

Because the VFB pin is a relatively high impedance node, stray capacitances at this pin must be minimized. Special attention should be given to any stray capacitances that
can couple external signals onto the pin, which can produce undesirable output transients or ripple. Effects of parasitic capacitance can typically be reduced by adding
a small-value (20pF to 50pF) feed forward capacitor from the BAT pin to the VFB pin

### Input Supply Voltage Regulation

The LT3652 contains a voltage monitor pin that enables programming a minimum operational voltage. Connecting a resistor divider from VIN to the VIN_REG pin enables programming of minimum input supply voltage, typically used to program the peak power voltage for a solar panel. Maximum charge current is reduced when the VIN_REG pin is below the regulation threshold of 2.7V. If an input supply cannot provide enough power to satisfy
the requirements of an LT3652 charger, the supply voltage will collapse. A minimum operating supply voltage can thus be programmed by monitoring the supply through a resistor divider, such that the desired minimum voltage corresponds to 2.7V at the VIN_REG pin. The LT3652 servos the maximum output charge current to maintain
the voltage on VIN_REG at or above 2.7V. Programming of the desired minimum voltage is accomplished by connecting a resistor divider as shown in
Figure 6. The ratio of RIN1/RIN2 for a desired minimum voltage (VIN(MIN)) is:

${\frac{R_{IN1}}{R_{IN2}}=\left(\frac{V_{IN\left(MIN\right)}}{2.7}\right)-1}$

If the voltage regulation feature is not used, connect the VIN_REG pin to VIN.

For my configuration I will set the ${V_{IN\left(MIN\right)}}$ as 10V 

${\frac{R_{IN1}}{R_{IN2}}=\frac{10}{2.7}-1=2.7037}$

${R_{IN1}=270k}$

${R_{IN2}=100k}$

### MPPT Temperature Compensation

### Battery Voltage Temperature Compensation

<i>See Page 16 of the datasheet for MPPT temperature compensation and battery voltage temperature compensation</i>

In this design I opted to ignore the following features to simplify the design. Nevertheless, these features are actually very easy to work with and are well documented within the datasheet.

### Status Pins

The LT3652 reports charger status through two open collector outputs, the CHRG and FAULT pins. These pins can accept voltages as high as VIN, and can sink up to 10mA when enabled. The CHRG pin indicates that the charger is delivering current at greater that a C/10 rate, or 1/10th of the programmed maximum charge current. The FAULT pin signals bad battery and NTC faults.

If the battery is removed from an LT3652 charger that is configured for C/10 termination, a sawtooth waveform of approximately 100mV appears at the charger output, due to cycling between termination and recharge events, This cycling results in pulsing at the CHRG output. An LED connected to this pin will exhibit a blinking pattern, indicating to the user that a battery is not present. The frequency of this blinking pattern is dependent on the output capacitance.

<p align="center">
    <img title="Status Pins Table" alt="Status Pins Table" src="./Images/Status_Pins.png" width ="50%">
</p>
<p align="center"><i>Status Pins Table</i></p>

### C/10 Termination

The LT3652 supports a low-current based termination scheme, where a battery charge cycle terminates when the current output from the charger falls to below one-tenth of the maximum current, as programmed with RSENSE. The C/10 threshold current corresponds to 10mV across RSENSE. This termination mode is engaged by shorting the TIMER pin to ground.
When C/10 termination is used, a LT3652 charger will source battery charge current as long as the average current level remains above the C/10 threshold. As the full-charge float voltage is achieved, the charge current falls until the C/10 threshold is reached, at which time the charger terminates and the LT3652 enters standby mode. The CHRG status pin follows the charger cycle, and is high impedance when the charger is not actively charging. When VBAT drops below 97.5% of the full-charged float voltage, whether by battery loading or replacement of the battery, the charger automatically re-engages and starts charging. There is no provision for bad battery detection if C/10 termination is used.

## Simulation
<p align="center">
    <img title="LTSpice Schematic Capture" alt="LTSpice Schematic Capture" src="./Simulations/LT3652_Schematic.png" width ="100%">
</p>
<p align="center"><i>LTSpice Schematic Capture</i></p>

<p align="center">
    <img title="LTSpice Schematic Capture" alt="LTSpice Schematic Capture" src="./Simulations/LT3652_Simulation_OUT.png" width ="100%">
</p>
<p align="center"><i>LTSpice Charge Simulation 0% --> 100% (Vin vs Vout)</i></p>

<p align="center">
    <img title="LTSpice CHG LED Status" alt="LTSpice CHG LED Status" src="./Simulations/LT3652_Simulation_LED_Status.png" width ="100%">
</p>
<p align="center"><i>LTSpice CHG LED Status</i></p>

### Layout Considerations

## PCB Design
<p align="center">
    <img title="KiCad Schematic Design" alt="KiCad Schematic Design" src="./Images/Schematic_Image.png" width ="75%">
</p>
<p align="center"><i>Schematic Design</i></p>
<p align="center">
    <img title="KiCad PCB Design" alt="KiCad PCB Design" src="./Images/PCB_Image.png" width ="75%">
</p>
<p align="center"><i>PCB Design</i></p>

## Final BOM 📃
* 2× [ICR18650 2600mAh 3.7V by PKCELL](https://www.batterypkcell.com/pkcell-18650-3-7v-2200mah-new-rechargeable-lithium-battery-product/)
* [12V 4.2W Solar Panel Module](https://www.amazon.ca/Portable-Polysilicon-Battery-Charger-Efficiency/dp/B07S1P67HL/ref=sr_1_97_sspa?crid=2WAO18NW0X2GN&dib=eyJ2IjoiMSJ9.fZVIoLELaw3YZhhkogK5VGuvzYc6wwH_Px50gEwUL5AK6Nxr_yeX3NtH-g3Q1hlIK30yAnZ0evMQHhDVoLYAm4sbS6SBN7zU1qKFeF3U7BU.zWxxjvWhspQamoYMqpsdiUn7VfXNYG4ixH8fwb_Tuoc&dib_tag=se&keywords=solar+cell&qid=1717475335&s=hi&sprefix=solar+ceil%2Ctools%2C96&sr=1-97-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGZfbmV4dA&psc=1)
* [Renogy 10 Watt 12 Volt Monocrystalline Solar Panel](https://www.amazon.ca/Renogy-Monocrystalline-Compact-Sprinkler-Applications/dp/B084MGS7KC/ref=mp_s_a_1_3?dib=eyJ2IjoiMSJ9.c6Ibopwzdw26-vXq0iM-t1X8KARw0PDHBEDa3QlSnucRRHnDQTm4tTqQhkWwJL5nBQfEcQG8s4SG3f0IUzD3oGxv7BOgHw_WdngOzWXWF6KB_JtXohbMmgxpaDqCVNrzySHuhJVVXBAElseQOjCSJATDCa9YTwj-vGisQsCN-2uRbedcTW2VFvSyFnXtX7MV--3pARRPw2olqf5FF4tQuw.rWuqIR5JHi2lb0LAExF7YcRQJTtzPzDO3Edo9STDuG8&dib_tag=se&keywords=10%2Bwatt%2Bsolar%2Bpanel&qid=1720388517&sr=8-3&th=1) (More powerful solar panel alternative)
    * [Datasheet](https://ca.renogy.com/content/renogy-day-2023/RNG-10D-SS-G1Datasheet.pdf)
    * Optimum Operating Voltage - 18V
    * Optimum Operating Current - 0.56A
* [LT3652EMSE#TRPBF - IC BATT CHG MULTI-CHEM 12MSOP](https://www.digikey.ca/en/products/detail/analog-devices-inc/LT3652EMSE-TRPBF/7838158)
* 2× [B5817WS-TP - DIODE SCHOTTKY 20V 1A SOD323](https://www.digikey.com/en/products/detail/micro-commercial-co/B5817WS-TP/2213553)
* [MBRS340T3G - DIODE SCHOTTKY 40V 3A SMC](https://www.digikey.com/en/products/detail/onsemi/MBRS340T3G/918009)
* 2× [150060VS75000 - LED GREEN CLEAR 0603 SMD](https://www.digikey.ca/en/products/detail/w%C3%BCrth-elektronik/150060VS75000/4489906)
* 2× [ERJ-3EKF1101V - RES SMD 1.1K OHM 1% 1/10W 0603](https://www.digikey.ca/en/products/detail/panasonic-electronic-components/ERJ-3EKF1101V/196037)
* 2× [CL10A106MA8NRNC - CAP CER 10UF 25V X5R 0603](https://www.digikey.ca/en/products/detail/samsung-electro-mechanics/CL10A106MA8NRNC/3887527)
* 2× [CL10A105KA8NNNC - CAP CER 1UF 25V X5R 0603](https://www.digikey.ca/en/products/detail/samsung-electro-mechanics/CL10A105KA8NNNC/3886760)
* [CRL0603-FW-R200ELF - RES 0.2 OHM 1% 1/8W 0603](https://www.digikey.ca/en/products/detail/bourns-inc/CRL0603-FW-R200ELF/3784480)
* [744772200 - FIXED IND 20UH 2.6A 50 MOHM TH](https://www.digikey.ca/en/products/detail/w%C3%BCrth-elektronik/744772200/3073923)
* [RC0603FR-07412KL - RES 412K OHM 1% 1/10W 0603](https://www.digikey.ca/en/products/detail/yageo/RC0603FR-07412KL/727227)
* [ERJ-3EKF3303V - RES SMD 330K OHM 1% 1/10W 0603](https://www.digikey.ca/en/products/detail/panasonic-electronic-components/ERJ-3EKF3303V/1746380)
* [ERJ-3EKF6802V - RES SMD 68K OHM 1% 1/10W 0603](https://www.digikey.ca/en/products/detail/panasonic-electronic-components/ERJ-3EKF6802V/1746444)
* [ERJ-3EKF1003V - RES SMD 100K OHM 1% 1/10W 0603](https://www.digikey.ca/en/products/detail/panasonic-electronic-components/ERJ-3EKF1003V/196075)
* [ERJ-3EKF2703V - RES SMD 270K OHM 1% 1/10W 0603](https://www.digikey.ca/en/products/detail/panasonic-electronic-components/ERJ-3EKF2703V/1746359)



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