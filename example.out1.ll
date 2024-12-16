declare i32 @getint()
declare i32 @getch()
declare float @getfloat()
declare i32 @getarray(ptr)
declare i32 @getfarray(ptr)
declare void @putint(i32)
declare void @putch(i32)
declare void @putfloat(float)
declare void @putarray(i32,ptr)
declare void @putfarray(i32,ptr)
declare void @_sysy_starttime(i32)
declare void @_sysy_stoptime(i32)
declare void @llvm.memset.p0.i32(ptr,i8,i32,i1)
declare i32 @llvm.umax.i32(i32,i32)
declare i32 @llvm.umin.i32(i32,i32)
declare i32 @llvm.smax.i32(i32,i32)
declare i32 @llvm.smin.i32(i32,i32)
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    %r1 = add i32 0,0
    %r3 = call i32 @getint()
    %r5 = add i32 0,0
    %r7 = add i32 0,0
    br label %L2
L2:  ;
    %r26 = phi i32 [%r5,%L1],[%r22,%L6]
    %r25 = phi i32 [%r7,%L1],[%r19,%L6]
    %r10 = icmp slt i32 %r26,%r3
    br i1 %r10, label %L3, label %L4
L3:  ;
    %r12 = add i32 10,0
    %r13 = icmp eq i32 %r26,%r12
    br i1 %r13, label %L5, label %L6
L4:  ;
    ret i32 %r25
L5:  ;
    %r15 = add i32 2,0
    %r16 = mul i32 %r25,%r15
    br label %L6
L6:  ;
    %r24 = phi i32 [%r25,%L3],[%r16,%L5]
    %r18 = add i32 2,0
    %r19 = add i32 %r24,%r18
    %r21 = add i32 1,0
    %r22 = add i32 %r26,%r21
    br label %L2
}
