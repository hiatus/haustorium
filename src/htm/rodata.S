.section .rodata, "a"
.balign 8

.global _rsh_start
.global _rsh_end

_rsh_start:
	.incbin RSHBIN
_rsh_end:
