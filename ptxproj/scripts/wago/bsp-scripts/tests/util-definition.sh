bc(){
    true
}
export -f bc

cp(){
    echo "cp $*"
}
export -f cp

rm(){
    echo "rm $*"
}
export -f rm 

ptxdistdummy()
{
    echo "./p $*"
}
export -f ptxdistdummy

pushd()
{
    echo "pushd $*"
}
export -f pushd

popd()
{
    echo "popd $*"
}
export -f popd

make()
{
    echo "make $*"
}
export -f make

mv()
{
    echo "mv $*"
}
export -f mv

bash()
{
    echo "bash $*"
}
export -f bash

# some tests require a real mkdir
#mkdir()
#{
#    echo "mkdir $*"
#}
#export -f mkdir

oneTimeTearDown(){
    unset rm ptxdistdummy pushd popd make mv bc 
}
export -f oneTimeTearDown
