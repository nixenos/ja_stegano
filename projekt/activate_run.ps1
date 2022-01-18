
$env:CONAN_OLD_conanrunenv_PATH=$env:PATH

foreach ($line in Get-Content "/home/nixen/Projects/JA/ja_stegano/projekt/environment_run.ps1.env") {
    $var,$value = $line -split '=',2
    $value_expanded = $ExecutionContext.InvokeCommand.ExpandString($value)
    Set-Item env:\$var -Value "$value_expanded"
}

function global:_old_conan_conanrunenv_prompt {""}
$function:_old_conan_conanrunenv_prompt = $function:prompt
function global:prompt {
    write-host "(conanrunenv) " -nonewline; & $function:_old_conan_conanrunenv_prompt
}