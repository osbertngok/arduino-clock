# Arduino Clock

This is an example Arduino program to display a clock on an I2C LCD1602.
You also need a switch to provide a digital input at DI7 for time adjustment.

## Available Operations:
1. Short Press: increment hour by 1;
2. Long Press: increment hour by 1 as holding;
3. Short Press \* 2: increment minute by 5;
4. Short Press then Long Press: Increment minute by 5 as holding;
5. Short Press \* 3: reset to 0.
