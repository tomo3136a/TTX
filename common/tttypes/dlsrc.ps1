$lastver="5_0"
$files=@("teraterm.h","tttypes.h")
$url="https://ja.osdn.net/projects/ttssh2/scm/svn/blobs/head/tags/teraterm-{0}/teraterm/common/{1}?export=raw"

function get_file($file, $vers){
    if (Test-Path ("lst_{0}" -f $file)){ $vers=gc ("lst_{0}" -f $file) }
    $vers|%{
        $f="./_source/v{0}_{1}" -f $_,$file
        if (-not (Test-Path $f)){ Invoke-WebRequest ($url -f $_,$file) -OutFile $f }
    }    
}
function copy_file($file, $ver){
    cp ../../../teraterm/common/${file} ./_source/v${ver}_${file}
}

mkdir _source -Force
cp ./source/*.h ./_source

get_file $files[0] ((63..106)|%{"4_"+$_})
get_file $files[1] ((63..106)|%{"4_"+$_})

copy_file $files[0] $lastver
copy_file $files[1] $lastver

