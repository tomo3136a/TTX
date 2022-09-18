$lastver="5_0"
$files=@("teraterm.h","tttypes.h")
$base="https://ja.osdn.net/projects/ttssh2/scm/svn/blobs/head/tags"
$url1="${base}/TERATERM_R{0}_RTM/source/common/{1}?export=raw"
$url2="${base}/teraterm-{0}/teraterm/common/{1}?export=raw"

function get_file1($file){
    $f="./_source/v2_6_{0}" -f $file
    if (-not (Test-Path $f)){
        "Download ${f}" | Out-Host
        Invoke-WebRequest ($url1 -f 206,$file) -OutFile $f
    }
}
function get_file2($file, $vers){
    if (Test-Path ("{0}.ver" -f $file)){ $vers=gc ("{0}.ver" -f $file) }
    $vers|?{$_[0] -ne '#'}|%{$_ -replace "\.","_"}|%{
        $f="./_source/v{0}_{1}" -f $_,$file
        if (-not (Test-Path $f)){
            "Download ${f}" | Out-Host
            Invoke-WebRequest ($url2 -f $_,$file) -OutFile $f
        }
    }
}
function copy_file($file, $ver){
    cp ../../../teraterm/common/${file} ./_source/v${ver}_${file} -Force
}

mkdir _source -Force | Out-Null
cp ./base/*.h ./_source

#$files | %{get_file1 $_ @(206)}
$files | %{get_file2 $_ ((63..106)|%{"4_"+$_})}
$files | %{copy_file $_ $lastver}
