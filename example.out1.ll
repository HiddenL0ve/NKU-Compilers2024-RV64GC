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
    %r2 = add i32 0,0
    %r4 = add i32 0,0
    %r5 = add i32 0,0
    br label %L2
L2:  ;
    %r13 = phi i32 [%r2,%L1],[%r11,%L3]
    %r7 = add i32 100,0
    %r8 = icmp slt i32 %r13,%r7
    br i1 %r8, label %L3, label %L4
L3:  ;
    %r10 = add i32 1,0
    %r11 = add i32 %r13,%r10
    br label %L2
L4:  ;
    ret i32 %r5
}
