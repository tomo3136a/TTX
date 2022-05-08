#!powershell -Sta -NonInteractive -NoProfile -NoLogo -ExecutionPolicy RemoteSigned %~dpn0.ps1 %*

$default_define=@{"MAX_PATH"=260;"MAXPATHLEN"=256;"LF_FACESIZE"=32;"MAX_UIMSG"=1024;}
$type_lst=@("tttset","TComVar")   #,"TGetHNRec","TKeyMap"
$out_file="../ttxversion_db.h"

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
function CStructMember($v){
    $v=$v -replace "\]\s+\[","][" -replace "FAR\s*\*","*"
    $v=$v -replace "\s+[*]\s*|[*]\s+","* " -replace "\s*,\s*",","
    $v=$v -replace "\s+"," "
    $v=$v -replace "(const|unsigned|struct|enum)\s+","`$1."
    if($v -match "\(\*(\w+)"){
        $v=$v -replace $Matches[1],"" -replace "\s+\w+","" -replace "\s+",""
        $v=$v+" "+$Matches[1]
        if($v){$v}
    }else{
        $vs=$v.Split(" ,")
        $vs[1..$vs.count]|%{"{0} {1}" -f $vs[0],$_}
    }
}
filter Get-CDefine($m_base){
    begin{$m=$m_base.clone()}
    process{if($_ -match "^#define\s+(\w+)\s+([^/]+)") {$m[$Matches[1]]=$Matches[2].trim()}}
    end{$m}
}
filter Set-CDefine($m){
    $s=$_;while($s -match "\[[^A-Za-z]*([A-Za-z]\w+)"){$n=$Matches[1];$s=$s -replace $n,$m[$n]}
    if($s -match "\[([^\]]+)\]"){$n=$Matches[1];$s -replace ($n -replace "([()*-+])","`\`$1"),(Invoke-Expression $n)}else{$s}
}
filter Remove-Else{begin{$s=0}process{if($_ -match "#else"){$s=1}elseif($_ -match "#endif"){$s=0}elseif($s -eq 0){$_}}}
filter Get-CStruct($types){begin{$old=$null}process{
    $a=((gc $_.pa -Encoding Oem)+(gc $_.pb -Encoding Oem))|Remove-Else
    $b=$a|Get-CStructName
    $c=$a|Get-CDefine $default_define
    $d=$a|%{$st=0;$i=0}{
        if($_ -match "struct[^{]*{"){$st=1
        }elseif($st -and $_ -match "}"){$st=0;$i=$i+1
        }elseif($st -and $_ -match "^\s*([^/;#]+)"){
            CStructMember($Matches[1].Trim())|%{"{0} {1}" -f $b[$i],$_}
        }
    }
    $e=$types|%{$f=$_;$d|?{$_ -match "^${f}"}}
    $g=$e|Set-CDefine($c)
    if (-not $old -or (Compare-Object $old $g)){@{v=$_.v;d=$g}}
    $old=$g
}}
filter Get-CStructCode{
    $v=$_.v
    $_.d|%{$st=$null}{
        $ss=$_.Split(" ",2)
        if ($st -ne $ss[0]){
            if ($st){"} ${st}_${v};";""}
            "typedef struct {"
        }
        $s1=$ss[1] -replace "\."," " -replace "(eterm_lookfeel_t|cygterm_t)","`$1_${v}"
        if($s1 -match "\(\*\)"){
            $s2=$s1 -split " ";
            $s3=$s2[-1];
            $s1=($s2[0..($s2.count-2)] -join " ") -replace "\(\*\)","(*${s3})"
        }
        "    {0};" -f $s1
        $st=$ss[0]
    }{if ($st){"} ${st}_${v};";""}}
}
filter Get-CStructType($t){if($_ -match "}\s*(${t}_\w+)\s*;"){$Matches[1]}}
filter Get-CUnion($t){begin{"typedef union {"}process{
    if($_ -match "\d+"){$m=$Matches[0];"    ${_} v${m};"}
}end{"} u_${t};";""}
}

filter Get-CMacro($t,$nm){
    begin{$v1=0;$v2=0}
    process{if($_ -match "\d+"){$v2=$v1;$v1=$Matches[0];}}
    end{
        $s=if($v2){$t2=$t -replace "\d+",$v2;"(rv>=${v2})?${t2}(rv,p,mb):"}else{""}
        "#define ${t}(rv,p,mb) (${s}&(((u_${nm}*)(p))->v${v1}.##mb))";
    }
}

function make_source($src){
    "// generated {0}. " -f (Get-Date -Format "yyyy-MM-dd")
    @("","#pragma once","#include <windows.h>","")
    $src
    $type_lst|%{
        $t1=$_;$t2=$_.ToUpper()
        $m=$src|Get-CStructType $t1|sort -Descending
        $m|Get-CUnion $t1
        $m|%{$col=@();$tt=""}{$tt=$_.ToUpper();$col+=$_;$col|Get-CMacro $tt $t1}{
            $tt2=$tt -replace "\d+","0";"#define ${tt2}(rv,p,mb) ${tt}(rv,p,mb)";
        }
        "#define ${t2}_PTR(rv,p,mb,c) (${t2}_##c(rv,p,mb))";
        "#define ${t2}(rv,p,mb,c) (*${t2}_PTR(rv,p,mb,c))";"";
    }
}

## main program
$file_lst=ls ./source/v*.h
$lst=$file_lst|%{$_.fullname}|Get-SourceVersion|sort -Unique|Get-SourceSet "./source"
$col1=$lst|Get-CStruct @("eterm_lookfeel_t","cygterm_t","tttset")
$col2=$lst|Get-CStruct @("TComVar")
$src=($col1|Get-CStructCode)+($col2|Get-CStructCode)
make_source($src)|Out-File $out_file

##add version checker
$m=@{};($col1+$col2)|sort {$_.v} -Descending|%{$v=$_.v;$_.d|%{
    $ss=$_ -split " "
    $t=$ss[0].ToUpper()
    $n=$ss[-1] -replace "\[.*\]"
    $m[($t+"_"+"VER_"+$n)]=$v
}}
$m.Keys|%{"#define {0} {1}" -f $_,$m[$_]}|sort|Out-File $out_file -Append
""|Out-File $out_file -Append

@("eterm_lookfeel_t","cygterm_t","tttset","TComVar")|%{
    $t=$_.ToUpper()
    "#define TEST_${t}(v,mb) (v>=${t}_VER_##mb)"
}|Out-File $out_file -Append
""|Out-File $out_file -Append
