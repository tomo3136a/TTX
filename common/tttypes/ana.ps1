#!powershell -Sta -NonInteractive -NoProfile -NoLogo -ExecutionPolicy RemoteSigned %~dpn0.ps1 %*

$int_size=4  #4:32bit-build, 8:64bit-build
$type_size=@{
    "BOOL"=$int_size;"BYTE"=1;"char"=1;"char*"=$int_size;"COLORREF"=$int_size;
    "const_wchar_t*"=$int_size;"DWORD"=4;"HANDLE"=$int_size;"HFONT"=$int_size;
    "HWND"=$int_size;"int"=$int_size;"int*"=$int_size;"_locale_t"=4;"LONG"=4;
    "NOTIFYICONDATA*"=$int_size;"PCHAR"=$int_size;"POINT"=8;"SIZE"=8;"size_t"=4;
    "struct_addrinfo*"=$int_size;"time_t"=8;"UINT_PTR"=$int_size;
    "unsigned_int"=$int_size;"void(*)(BYTE)"=$int_size;"void(*)(int)"=$int_size;
    "void*"=$int_size;"wchar_t*"=$int_size;"WORD"=2;"HINSTANCE"=$int_size;
    "TTTSet*"=$int_size;"wchar_t"=2;
}
$default_define=@{"MAX_PATH"=260;"MAXPATHLEN"=256;"LF_FACESIZE"=32;"MAX_UIMSG"=1024;}
$struct_lst=@("eterm_lookfeel_t","cygterm_t","tttset","TGetHNRec","TComVar")

#### source set
filter Get-SourceVersion{
    if($_ -match "v(\d+)_(\d+)([A-Za-z])?_\w+\.h"){(0+$Matches[1])*1000+(0+$Matches[2])}
}
filter Get-SourceSet ($path){begin{$va=0;$vb=0}process{
    $v1=[int]($_/1000);$v2=$_%1000;
    if(Test-Path ("${path}/v${v1}_${v2}*_teraterm.h")){$va=$_}
    if(Test-Path ("${path}/v${v1}_${v2}*_tttypes.h")){$vb=$_}
    $va1=[int]($va/1000);$va2=$va%1000;
    $vb1=[int]($vb/1000);$vb2=$vb%1000;
    $pa="${path}/v${va1}_${va2}_teraterm.h"
    $pb="${path}/v${vb1}_${vb2}_tttypes.h"
    @{v=$_;pa=$pa;pb=$pb}
}}
####
filter Get-CStructName{begin{$s=0;$n=""}process{
    if($_ -match "struct[^{]*{"){$s=1;$n=$_
    }elseif($s -and $_ -match "}"){$s=0;
        if(($n+$_) -match "(\w+)([\{\};]|\s)*$"){$Matches[1]}}
}}
filter Remove-CComment{begin{$s=0}process{$v=$_
    if($s -eq 1){if($v -match "\*/"){$v=$v -replace "^.*\*/","";$s=0}}
    if($s -eq 0){
        $v=$v -replace "//.*$","" -replace "/\*.*\*/",""
        if ($v -match "/\*"){$s=1;$v=$v -replace "/\*.*$",""}
        $v=$v.trim()
        if($v.length -ne 0){$v}
    }
}}
filter Remove-CElse{begin{$s=0}process{
    if($_ -match "#else"){$s=1}elseif($_ -match "#endif"){$s=0}elseif($s -eq 0){$_}
}}
filter Format-CStruct($col=@()){begin{$s=0;$i=0}process{
    if($_ -match "struct[^{]*{"){$s=$s+1
    }elseif($s -and $_ -match "}"){$s=$s-1;if($s -eq 0){$i=$i+1}
    }elseif($s -and $_ -match "^\s*([^/;#]+)"){$v=$Matches[1].trim();$vv=$v
        $v=$v -replace "FAR\s*\*","*"
        $v=$v -replace "(const|unsigned|struct|enum)\s+(\w+)","`$1_`$2"
        $v=$v -replace "\s+(\*+)","`$1 "
        $t,$v=$v -split " ",2
        if($v -match "\(\*(\w+)"){
             $t=$t+($v -replace $Matches[1],"" -replace "\s+\w+","" -replace "\s+","")
             $v=$Matches[1]
        }
        $v=$v -replace "\s+",""
        $v.Split(",")|%{$col[$i]+" "+$t+" "+$_}
    }
}}
filter Format-CArray{$_ -replace "(\w+)\s(\w+)\[((?:\d+|\w+|[-()*+])+)\]","`$1 `$2[] `$3" -replace "\[(\w+)\]","*`$1"}
filter Set-CSize($f){begin{$s="";$i=0}process{
    $num=$_.number -replace "sizeof\(.+\*\)",$int_size
    if($num -match "[A-Za-z]\w+"){$n=$Matches[0];$num=$num -replace $n,$f[$n]}
    if($num -match "[A-Za-z]\w+"){$n=$Matches[0];$num=$num -replace $n,$f[$n]}
    if($num -match "[A-Za-z]\w+"){$n=$Matches[0];$num=$num -replace $n,$f[$n]}
    if($num -match "[A-Za-z]\w+"){$n=$Matches[0];$num=$num -replace $n,$f[$n]}
    if($num -match "[A-Za-z]\w+"){$n=$Matches[0];$num=$num -replace $n,$f[$n]}
    if($num){$_.number=$num=Invoke-Expression $num}
    $_.type_size=if($_.type -match "^enum_"){4}else{$type_size[$_.type]}
    $_.size=if($_.number){0+$_.number*$_.type_size}else{0+$_.type_size}
    $_.adj=if($_.type_size -lt 4){0+$_.type_size}else{4}
    if($_.struct -ne $s){$s=$_.struct;$i=0}
    if($_.adj -eq 0){$_.name|Out-Host}
    if($i%$_.adj){$i=$i-$i%$_.adj+$_.adj}
    $_.offset=$i;$i=$i+$_.size
    $_.next=$_.offset+$_.size
    $type_size[$_.struct]=$_.next
}}
filter Get-CDefine($m_base){
    begin{$m=$m_base.clone()}
    process{if($_ -match "^#define\s+(\w+)\s+([^/]+)") {$m[$Matches[1]]=$Matches[2].trim()}}
    end{$m}
}
filter Set-CDefine($m){
    $s=$_;while($s -match "\[[^A-Za-z]*([A-Za-z]\w+)"){$n=$Matches[1];$s=$s -replace $n,$m[$n]}
    if($s -match "\[([^\]]+)\]"){$n=$Matches[1];$s -replace ($n -replace "([-()*+])","`\`$1"),(Invoke-Expression $n)}else{$s}
}
filter Export-CStructCsv($outpath){begin{$old=$null}process{
    $a=((gc $_.pa -Encoding Oem)+(gc $_.pb -Encoding Oem))|Remove-CComment|Remove-CElse
    $b=$a|Get-CStructName
    $c=$a|Format-CStruct $b
    $c=$c|Format-CArray
    $d=$c|ConvertFrom-Csv -Delimiter " " -Header "struct","type","name","number"|?{$_.struct -ne "TMap"}
    $e=$d|select struct,name,type,type_size,number,size,adj,offset,next
    $f=$a|Get-CDefine $default_define
    $e|Set-CSize $f
    $g=$struct_lst|%{$v=$_;$e|?{$_.struct -eq $v}}
    #if (-not $old -or (Compare-Object $old $g)){
    if (-not $old -or $true){
        $_.v|Out-Host
        $v1=[int]($_.v/1000);$v2=$_.v%1000;
        $g|select "struct","type","offset","size","name"|Export-Csv "${outpath}/v${v1}_${v2}.csv" -NoTypeInformation    
    }
    $old=$g
}}

## main program
$file_lst=ls ./_source/v*.h
$version_lst=$file_lst|%{$_.fullname}|Get-SourceVersion|sort -Unique
mkdir ./_data -Force|Out-Null
$version_lst|Get-SourceSet "./_source"|Export-CStructCsv "./_data"
