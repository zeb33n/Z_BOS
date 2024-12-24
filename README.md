# TacOS
Here is the bug. 
I think the string literall defined in `src/kernel/kernel.cpp` is being overwritten by the isr definitions in `src/kernel/idt.asm`. 

![image](https://github.com/user-attachments/assets/dbe24796-577c-4c5d-a0b8-573411eb3d4f)

I can alter how much of the string is truncated by commenting out isrs in idt.asm. 

![image](https://github.com/user-attachments/assets/cbe5981f-e3a0-4614-a958-14486bf07a0b)

more isrs commented. 

![image](https://github.com/user-attachments/assets/d7a75eaa-c6bf-499e-85ff-bc223c463409)

less isrs commented

Weird. I tried printing out the memory address of the isrs and couldn't think of any obvious overlap. Maybe something todo with segments? 
Run the project using `run.sh` assuming you have a cross compiler set up. 
