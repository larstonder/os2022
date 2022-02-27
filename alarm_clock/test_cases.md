#Test cases

## Test case 1 - List alarms and add alarm

1. Start Alarm Clock
2. Press 'l' to list all current alarms
Expected result: Print "No alarms are scheduled"
3. Press 's' to schedule an alarm
4. Input a date after current time
5. Press 'l' to list all current alarms
Expected result: Print the inputted alarm.
6. Wait until the inputted date
Expected result: Print "Ring ring" and play alarm sound

## Test case 2 - Multiple alarm scheduling

1. Start Alarm Clock
2. Press 's' to schedule an alarm
3. Input a date after current time
4. Press 'l' to list all current alarms
Expected result: Print the single inputted alarm
5. Press 's' to schedule an alarm
6. Input another date after current time
7. Press 'l' to list all current alarms
Expected result: List both alarms if not one alarm has rung already

## Test case 3 - Alarm list overflow and deletion

1. Start Alarm Clock
2. Press 's' to schedule an alarm
3. Input a date after current time
4. Repeat step 2. and 3. nine more times.
5. Press 'l' to list all current alarms
Expected result: Print all set alarms
6. Repeat step 2. and 3. to set a new alarm
Expected result: Print that the alarm cannot be set as maximum number of alarms have been reached.
7. Press 'c' to cancel an alarm
8. Input which alarm to cancel
10. Press 'l' to list all current alarms
Expected result: Selected alarm is canceled
9. Repeat step 2. and 3. to set a new alarm
10. Press 'l' to list all current alarms
Expected result: New alarm is set in place of the canceled one

## Test case 4 - Alarm process deletion

1. Start Alarm Clock
2. Press 's' to schedule an alarm
3. Input a date before current time
Expected result: Print that the alarm must be after current time
4. Press 'l' to list all current alarms
Expected result: No alarms are printed
5. Press 's' to schedule an alarm
6. Input a date after current time
7. Press 'c' to cancel an alarm
8. Input a number over 1
Expected result: No alarm is canceled as there is only one alarm
9. Press 'c' to cancel an alarm
10. Input 1
11. Wait until the time the alarm was set to
Expected result: Nothing happends