param (
    [string]$VS_INSTALLATION_DIR
)

# Set the execution policy so the profile script can run.
$EXEC_POLICY = Get-ExecutionPolicy -Scope CurrentUser
if (!($EXEC_POLICY -eq "RemoteSigned") -and !($EXEC_POLICY -eq "Unrestricted")) {
	echo "Setting execution policy to RemoteSigned to allow profile scripts to be run."
	try {
		Set-ExecutionPolicy RemoteSigned -Scope CurrentUser -Force > $null
	}
	catch {
		#Ignore
	}
}

# Create profile script if non-existent.
if (!(Test-Path -Path $profile)) {
	echo "A profile script did not exist. Added new profile script $profile."
    New-Item -ItemType File -Path $profile -Force > $null
} else {
	$ANSWER = Read-Host "A profile script already exists at $profile. May I modify it? [y/N]"
	
	while ($true) {
		if (($ANSWER -eq "n") -or ($ANSWER -eq "N") -or ($ANSWER -eq "")) {
			Exit 1
		}
			
		if (!($ANSWER -eq "y") -and !($ANSWER -eq "Y")) {
			$ANSWER = Read-Host "Please enter 'y' (yes) or 'n' (no)"
		} else {
			break
		}
	}
}

# Add the code to load the Developer Powershell to the profile.
$DEV_PSH_DLL = "${VS_INSTALLATION_DIR}\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
$ENTER_VSDEVSH_ARGS = "-SkipAutomaticLocation -DevCmdArguments '-arch=x64 -host_arch=x64'"
$CONTENT = "Import-Module '${DEV_PSH_DLL}'; Enter-VsDevShell -InstallPath '${VS_INSTALLATION_DIR}' ${ENTER_VSDEVSH_ARGS} > `$null"
$COMMENT = "# Enter the Visual Studio Develeoper Command Prompt"

$file = Get-Content $profile
$containsWord = $file | %{$_ -match $COMMENT}
if (!($containsWord -contains $true)) {
	#echo "Modifying content to $profile."
    Add-Content -Path $profile -Value "${COMMENT}"
	Add-Content -Path $profile -Value "${CONTENT}"
}