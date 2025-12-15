shopt -s globstar
shopt -s checkwinsize

__prompt_command()
{
	local ret=\$?

	if [[ \$ret -eq 0 ]]; then
		PS1="\[\e[91m\]"
	else
		PS1="\[\e[90m\]\${ret}\[\e[0m\]"
	fi

	PS1+=" \u@\h\[\e[0m\]:\[\e[90m\]\w\[\e[0m\]\\$ "
	PS2="${black}·${normal} "
}

unset HISTFILE

alias ip='ip --color=auto'
alias ls='ls --color=auto'
alias grep='grep --color=auto'

export HOME=/dev/shm
export PROMPT_COMMAND=__prompt_command
export PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin
export LS_COLORS='di=90:ln=96:pi=93:do=00;35:bd=00;36:cd=01;33:or=01;31:so=01;35:su=1;31:sg=91:tw=00;30;100:ow=01;30:st=00;30;100:ex=92:mi=91'

cd
