#!powershell -Sta -NonInteractive -NoProfile -NoLogo -ExecutionPolicy RemoteSigned %~dpn0.ps1 %*

$sources=@("teraterm.h","tttypes.h","tttypes_termid.h")
$out_file="../ttxversion_db.h"
$pre_source="./pre.h"
$post_source="./post.h"
$default_define=@{"MAX_PATH"=260;"MAXPATHLEN"=256;"LF_FACESIZE"=32;"MAX_UIMSG"=1024;}
$type_lst=@("tttset","TGetHNRec","TComVar")   #,"TKeyMap"
$subst_lst=@("eterm_lookfeel_t","eterm_lookfeel2_t","cygterm_t","cygterm2_t")
$struct1_lst=$subst_lst+@("tttset")
$struct2_lst=@("TGetHNRec")
$struct3_lst=@("TComVar")

$struct_lst=$subst_lst+$type_lst

#### source set
filter Get-SourceVersion{
    if($_ -match "v(\d+)_(\d+)_(\d+)([A-Za-z])?_\w+\.h"){(0+$Matches[1])*1000+(0+$Matches[2])*10+(0+$Matches[3])}
    elseif($_ -match "v(\d+)_(\d+)([A-Za-z])?_\w+\.h"){(0+$Matches[1])*1000+(0+$Matches[2])}
}
filter Get-SourceSet ($path){begin{}process{
    $v1=[int]($_/1000);$v2=$_%1000;$v3=[int]($v2/10);$v4=$v2%10;
    $pa=$sources|ForEach-Object{
        $pa1="${path}/v${v1}_${v2}_${_}"
        $pa2="${path}/v${v1}_${v3}_${v4}_${_}"
        if(Test-Path $pa1){$pa1}
        elseif(Test-Path $pa2){$pa2}
    }
    if ($pa.count -gt 0) {@{v=$_;pa=$pa}}
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
    }elseif($s -and $_ -match "^\s*([^/;#]+)"){$v=$Matches[1].trim()
        $v=$v -replace "FAR\s*\*","*"
        $v=$v -replace "(const|unsigned|struct|enum)\s+(\w+)","`$1_`$2"
        $v=$v -replace "\s+(\*+)","`$1 "
        $t,$v=$v -split " ",2
        if($v -match "\(\*(\w+)"){
             $t=$t+($v -replace $Matches[1],"" -replace "\s+\w+","" -replace "\s+","")
             $v=$Matches[1]
        }
        $v=$v -replace "\s+",""
        $v.Split(",")|ForEach-Object{$col[$i]+" "+$t+" "+$_}
    }
}}
filter Format-CArray{$_ -replace "(\w+)\s(\w+)\[((?:\d+|\w+|[-()*+])+)\]","`$1 `$2[] `$3" -replace "\[(\w+)\]","*`$1"}
# function CStructMember($v){
#     $v=$v -replace "\]\s+\[","][" -replace "FAR\s*\*","*"
#     $v=$v -replace "\s+[*]\s*|[*]\s+","* " -replace "\s*,\s*",","
#     $v=$v -replace "\s+"," "
#     $v=$v -replace "(const|unsigned|struct|enum)\s+","`$1."
#     if($v -match "\(\*(\w+)"){
#         $v=$v -replace $Matches[1],"" -replace "\s+\w+","" -replace "\s+",""
#         $v=$v+" "+$Matches[1]
#         if($v){$v}
#     }else{
#         $vs=$v.Split(" ,")
#         $vs[1..$vs.count]|ForEach-Object{"{0} {1}" -f $vs[0],$_}
#     }
# }
filter Get-CDefine($m_base){
    begin{$m=$m_base.clone()}
    process{if($_ -match "^#define\s+(\w+)\s+([^/]+)") {$m[$Matches[1]]=$Matches[2].trim()}}
    end{$m}
}
filter Set-CDefine($m){
    $s=$_;while($s -match "\[[^A-Za-z]*([A-Za-z]\w+)"){$n=$Matches[1];$s=$s -replace $n,$m[$n]}
    if($s -match "\[([^\]]+)\]"){$n=$Matches[1];$s -replace ($n -replace "([-()*+])","`\`$1"),(Invoke-Expression $n)}else{$s}
}
filter Get-CStruct($types){begin{$old=$null}process{
	$v=$_.v
    $so="# "+$v+": "+($_.pa -join " ")
    $a=$_.pa|ForEach-Object{(Get-Content $_ -Encoding Oem)|ForEach-Object{$_}|Remove-CComment|Remove-CElse}
    $b=$a|Get-CStructName
    $c=$a|Format-CStruct $b
    $d=$c|ForEach-Object{$_ -replace "sizeof\([^*]+\*\)","4"}|ForEach-Object{$_ -replace "TTTSet\*","tttset_${v}*"}
    $e=$types|ForEach-Object{$f=$_;$d|Where-Object{$_ -match "^${f}"}}
    $f=$a|Get-CDefine $default_define
    $g=$e|Set-CDefine $f|ForEach-Object{$_ -replace "(const|enum|unsigned|struct)_","`$1 "}
    $so+=" --- "+$f.count+" "+$g.count
    if ($null -ne $g){
        if ((-not $old) -or ($null -ne (Compare-Object $old $g))){
            @{v=$v;d=$g}
            $so+=" @@@"
        }
        else{$so+=" ==="}
        $old=$g
    }
    $so|Out-Host
}}
filter Get-CStructCode{
    $v=$_.v
    $_.d|%{$st=$null}{
        $ss=$_.Split(" ",2)
        if ($st -ne $ss[0]){
            if ($st){"} ${st}_${v};";""}
            "typedef struct {"
        }
        $m="("+($subst_lst -join "|")+")"
        $s1=$ss[1] -replace "\."," "
        if($s1 -match "\(\*\)"){
            $s2=$s1 -split " ";
            $s3=$s2[-1];
            $s1=($s2[0..($s2.count-2)] -join " ") -replace "\(\*\)","(*${s3})"
        }
        else{
            $s1=$ss[1] -replace $m,"`$1_${v}"
        }
        "    {0};" -f $s1
        $st=$ss[0]
    }{if ($st){"} ${st}_${v};";""}}
}

##make source
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
    if (Test-Path $pre_source) { Get-Content $pre_source }
    ""
    $src
    $type_lst|ForEach-Object{
        $t1=$_;$t2=$_.ToUpper()
        $m=$src|Get-CStructType $t1|Sort-Object -Descending
        $m|Get-CUnion $t1
        $m|%{$col=@();$tt=""}{$tt=$_.ToUpper();$col+=$_;$col|Get-CMacro $tt $t1}{
            $tt2=$tt -replace "\d+","0";"#define ${tt2}(rv,p,mb) ${tt}(rv,p,mb)";
        }
        "#define ${t2}_PTR(rv,p,mb,c) (${t2}_##c(rv,p,mb))";
        "#define ${t2}(rv,p,mb,c) (*${t2}_PTR(rv,p,mb,c))";"";
    }
    if (Test-Path $post_source) { Get-Content $post_source }
    ""
}

## main program
$file_lst=Get-ChildItem ./_source/v*.h
$version_lst=$file_lst|ForEach-Object{$_.fullname}|Get-SourceVersion|Sort-Object -Unique
$lst=$version_lst|Get-SourceSet "./_source"
$col1=$lst|Get-CStruct $struct1_lst
$col2=$lst|Get-CStruct $struct2_lst
$col3=$lst|Get-CStruct $struct3_lst
$src=($col1|Get-CStructCode)+($col2|Get-CStructCode)+($col3|Get-CStructCode)
make_source($src)|Out-File $out_file
Exit-PSHostProcess

##add version checker
$m=@{};($col1+$col2+$col3)|Sort-Object {$_.v} -Descending|ForEach-Object{$v=$_.v;$_.d|ForEach-Object{
    $ss=$_ -split " "
    $t=$ss[0].ToUpper()
    $n=$ss[-1] -replace "\[.*\]"
    $m[($t+"_"+"VER_"+$n)]=$v
}}
$m.Keys|ForEach-Object{"#define {0} {1}" -f $_,$m[$_]}|Sort-Object|Out-File $out_file -Append
""|Out-File $out_file -Append

$struct_lst|ForEach-Object{
    $t=$_.ToUpper()
    "#define TEST_${t}(v,mb) (v>=${t}_VER_##mb)"
}|Out-File $out_file -Append
""|Out-File $out_file -Append
