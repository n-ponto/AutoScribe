/*
Contains all code specific to the Manual Control runtime mode.
*/

void drawing()
{
    // Read 4 bytes from serial corresponding to a coordinate pair
    // Proccess the instructions from the coordinate pair and 
    // queue the instruction to be read by the interrupt
    
    // If the queue is full then stop reading
}

void drawing_interrupt()
{
    // Execute the previous command
    // If that's the end of the queded coordinate then unqueue the next coordinate
    // Figure out the next command on the way to the next coordinate
    // Save the next command 
}