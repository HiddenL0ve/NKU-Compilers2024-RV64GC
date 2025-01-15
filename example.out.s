	.text
	.globl main
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0"
main:
.main_0:
	jal			x0,.main_1
.main_1:
	addiw		t0,x0,5
	addiw		t0,x0,0
	addw		t0,t0,t0
	addiw		a0,x0,3
	jalr		x0,ra,0
	.data
