/*
Contains all code specific to the Manual Control runtime mode.

*/

void manual_control()
{
    // Start the timer interrupt with an interrupt function specificall for manual_control

    // Read a byte from serial corresponding to a direction (or moving pen up/down)
    // Convert the direction into a specific command (how to move which motors)
    // Buffer the more specific command so the interrupt can immediately read
    // and execute the command
    // If the queue is full then stop reading

    // When receives the stop signal, immediately stop the timer interrupt
    // Then change the mode back to accepting commands
}

void manual_control_interrupt()
{
    // Read a command from the buffer 
    // Execute that command
    // Set up the next command for the next interrupt
}