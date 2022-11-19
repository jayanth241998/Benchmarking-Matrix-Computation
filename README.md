### CS553 Cloud Computing Homework 3 Repo
**Illinois Institute of Technology**  

**Team Name**: Tanmay-Mithilesh-Jayanth-HW3-CC  
**Students**:  
* Tanmay Anand A20519843  (tanand4@hawk.iit.edu)  
* Jayanth Chidananda A20517012 (jchidananda@hawk.iit.edu)   
* Mithilesh Bade A20514157 (mbade@hawk.iit.edu)  

compilation for matrix

we have used the compiler flag -O3 while compiling the program only for large matrix multiplications not flops, which produces optimal results for matrix multiplication and which we recorded for our outputs

gcc -O3 cpubench.c -o cpubench -lpthread


WARNING NOTE 

using compiler flag -O3 for flops calculation will give incorrect results.

for loops the compilation command is 

gcc cpubench.c -o cpubench -lpthread

