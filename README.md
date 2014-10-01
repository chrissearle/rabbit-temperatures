# Monitoring the temperature of the rabbit cage/hutch

This is based around an arduino uno with two DS18B20 one wire temperature sensors for measuring and an
ethernet with poe shield (providing both network and power).

## Parts

### Temperature

This is the arduino sketch for the project.

Heavily based on [this tutorial from raywenderlich.com](http://www.raywenderlich.com/38841/arduino-tutorial-temperature-sensor)
but returning output suitable for munin instead of JSON.

### munin

Simple munin script that munin will use to poll the arduino.

### libs

Two libraries that are used in the arduino sketch. Need to be installed (unzipped) to Java/libraries in the
installed arduino environment - on a mac this means opening the arduino package and navigating down to:

    Contents > Resources > Java > libraries