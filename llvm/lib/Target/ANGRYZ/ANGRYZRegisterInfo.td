let Namespace = "ANGRYZ" in {

class ANGRYZReg128<bits<7> enc, string name, list<string> alt = [] >
    : Register<name> {
    let HWEncoding{6-0} = enc;
    let AltNames = alt;
}
def ABIRegAltName : RegAltNameIndex;

}



let RegAltNameIndices = [ABIRegAltName] in {
    foreach Index = 0-127 in {
        def H#Index# : ANGRYZReg128<Index, "h"#Index, ["h"#Index]>,
                       DwarfRegNum<[Index]>;
    }
}


/*
    class RegisterClass<string namespace, list<ValueType> regTypes, int alignment,
                    dag regList, RegAltNameIndex idx = NoRegAltName>
    第三个参数的alignment ： load/store的时候 它的地址必须是32bit，也就是4字节倍数
    


*/

class HPRRegisterClass<dag regList>
    :   RegisterClass<"ANGRYZ", [i32], 32, regList> {
    //let RegInfos = 
}

def HPR : HPRRegisterClass< (add (sequence "H%u", 0, 16), H127) >;