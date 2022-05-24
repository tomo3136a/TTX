$lastver="5_0"
$files=@("teraterm.h","tttypes.h")
$url1="https://ja.osdn.net/projects/ttssh2/scm/svn/blobs/head/tags/TERATERM_R{0}_RTM/source/common/{1}?export=raw"
$url2="https://ja.osdn.net/projects/ttssh2/scm/svn/blobs/head/tags/teraterm-{0}/teraterm/common/{1}?export=raw"

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
    cp ../../../teraterm/common/${file} ./_source/v${ver}_${file}
}

mkdir _source -Force | Out-Null
cp ./source/*.h ./_source

#get_file1 $files[0] @(206)
#get_file1 $files[1] @(206)

get_file2 $files[0] ((63..106)|%{"4_"+$_})
get_file2 $files[1] ((63..106)|%{"4_"+$_})

copy_file $files[0] $lastver
copy_file $files[1] $lastver

