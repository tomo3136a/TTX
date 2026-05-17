$files=@{
    "release_1.ver"=@("teraterm-", "_", @("teraterm.h", "tttypes.h"))
    "release_2.ver"=@("v", ".", @("teraterm.h", "tttypes.h"))
    "release_3.ver"=@("v", ".", @("teraterm.h", "tttypes.h", "tttypes_termid.h"))
}
$base="https://github.com/TeraTermProject/teraterm/raw/refs/tags"
$url="${base}/{0}{1}/teraterm/common/{2}"

function get_file($file, $kw, $vers, $c){
    $vers|Where-Object{$_[0] -ne '#'}|ForEach-Object{
        $f="./_source/v{0}_{1}" -f ($_ -replace "\.","_"),$file
        if (-not (Test-Path $f)){
            $p = $url -f $kw,($_ -replace "\.",$c),$file
            "Download ${f} from ${p}" | Out-Host
            Invoke-WebRequest $p -OutFile $f
        }
    }
}

mkdir _source -Force | Out-Null
Copy-Item ./base/*.h ./_source

$files.keys|ForEach-Object{$v=$_;$vs=$files[$_];$vs[2]|ForEach-Object{
    get_file $_ $vs[0] (Get-Content $v) $vs[1]
}}
