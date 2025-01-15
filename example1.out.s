	.text
	.globl main
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0"
main:
.main_0:
	addi		sp,sp,-16
	jal			x0,.main_1
.main_1:
	sd			fp,-8(sp)
	addiw		fp,x0,0
	lui			t0,%hi(a)
	lw			t0,%lo(a)(t0)
	lui			t1,%hi(b)
	lw			t1,%lo(b)(t1)
	mulw		t0,t0,t1
	lui			t1,%hi(c)
	lw			t1,%lo(c)(t1)
	divw		t0,t0,t1
	lui			t1,%hi(e)
	lw			t1,%lo(e)(t1)
	lui			t2,%hi(d)
	lw			t2,%lo(d)(t2)
	addw		t1,t1,t2
	beq			t0,t1,.main_5
	jal			x0,.main_4
.main_2:
	addiw		fp,x0,1
	ld			fp,-8(sp)
	jal			x0,.main_3
.main_3:
	sd			ra,0(sp)
	add			a0,fp,x0
	call		putint
	add			a0,fp,x0
	ld			ra,0(sp)
	addi		sp,sp,16
	jalr		x0,ra,0
.main_4:
	lui			t0,%hi(a)
	lw			t0,%lo(a)(t0)
	lui			t1,%hi(b)
	lw			t1,%lo(b)(t1)
	lui			t2,%hi(c)
	lw			t2,%lo(c)(t2)
	mulw		t1,t1,t2
	subw		t0,t0,t1
	lui			t1,%hi(d)
	lw			t1,%lo(d)(t1)
	lui			t2,%hi(a)
	lw			t2,%lo(a)(t2)
	lui			t3,%hi(c)
	lw			t3,%lo(c)(t3)
	divw		t2,t2,t3
	subw		t1,t1,t2
	beq			t0,t1,.main_2
	jal			x0,.main_6
.main_5:
	lui			t0,%hi(a)
	lw			t0,%lo(a)(t0)
	lui			t1,%hi(a)
	lw			t1,%lo(a)(t1)
	lui			t2,%hi(b)
	lw			t2,%lo(b)(t2)
	addw		t1,t1,t2
	mulw		t0,t0,t1
	lui			t1,%hi(c)
	lw			t1,%lo(c)(t1)
	addw		t0,t0,t1
	lui			t1,%hi(d)
	lw			t1,%lo(d)(t1)
	lui			t2,%hi(e)
	lw			t2,%lo(e)(t2)
	addw		t1,t1,t2
	ble			t0,t1,.main_2
	jal			x0,.main_4
.main_6:
	jal			x0,.main_3
	.data
a:
	.word	1
b:
	.word	0
c:
	.word	1
d:
	.word	2
e:
	.word	4
