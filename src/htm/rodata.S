.section .rodata, "a"
.balign 8

.global _rsh_start
.global _rsh_end
.global _bashrc_start
.global _bashrc_end

_rsh_start:
	.incbin RSHBIN
_rsh_end:
_bashrc_start:
	.incbin BRCBIN
_bashrc_end:
