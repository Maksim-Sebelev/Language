#ifndef GLOBAL_INCLUDE_HPP
#define GLOBAL_INCLUDE_HPP

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <stddef.h>
#include "lib/lib.hpp"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum Cmd : int
{
    hlt = 0    ,
    push       ,
    pop        ,
    add        ,
    sub        ,
    mul        ,
    dive       ,
    pp         , 
    mm         ,
    out        ,  // just out int
    outc       ,  // out char
    outr       ,  // out int and remove it
    outrc      ,  // out char and remove it
    jmp        ,
    ja         ,
    jae        ,
    jb         ,
    jbe        ,
    je         ,
    jne        ,
    call       ,
    ret        ,
    draw       ,
    rgba       ,
    CMD_QUANT  , // count
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct CmdInfo
{
    const Cmd    cmd;
    const char*  name; 
    const size_t argQuant;
    const size_t codeRecordSize;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const CmdInfo CmdInfoArr[] = 
{
    {Cmd::hlt  , .name = "hlt"  , .argQuant = 0, .codeRecordSize = 1},
    {Cmd::push , .name = "push" , .argQuant = 1, .codeRecordSize = 4},
    {Cmd::pop  , .name = "pop"  , .argQuant = 1, .codeRecordSize = 4},
    {Cmd::add  , .name = "add"  , .argQuant = 0, .codeRecordSize = 1},
    {Cmd::sub  , .name = "sub"  , .argQuant = 0, .codeRecordSize = 1},
    {Cmd::mul  , .name = "mul"  , .argQuant = 0, .codeRecordSize = 1},
    {Cmd::dive , .name = "div"  , .argQuant = 0, .codeRecordSize = 1},
    {Cmd::pp   , .name = "pp"   , .argQuant = 1, .codeRecordSize = 2},
    {Cmd::mm   , .name = "mm"   , .argQuant = 1, .codeRecordSize = 2},
    {Cmd::out  , .name = "out"  , .argQuant = 0, .codeRecordSize = 1},
    {Cmd::outc , .name = "outc" , .argQuant = 0, .codeRecordSize = 1},
    {Cmd::outr , .name = "outr" , .argQuant = 0, .codeRecordSize = 1},
    {Cmd::outrc, .name = "outrc", .argQuant = 0, .codeRecordSize = 1},
    {Cmd::jmp  , .name = "jmp"  , .argQuant = 1, .codeRecordSize = 2},
    {Cmd::ja   , .name = "ja"   , .argQuant = 1, .codeRecordSize = 2},
    {Cmd::jae  , .name = "jae"  , .argQuant = 1, .codeRecordSize = 2},
    {Cmd::jb   , .name = "jb"   , .argQuant = 1, .codeRecordSize = 2},
    {Cmd::jbe  , .name = "jbe"  , .argQuant = 1, .codeRecordSize = 2},
    {Cmd::je   , .name = "je"   , .argQuant = 1, .codeRecordSize = 2},
    {Cmd::jne  , .name = "jne"  , .argQuant = 1, .codeRecordSize = 2},
    {Cmd::call , .name = "call" , .argQuant = 1, .codeRecordSize = 2},
    {Cmd::ret  , .name = "ret"  , .argQuant = 0, .codeRecordSize = 1},
    {Cmd::draw , .name = "draw" , .argQuant = 2, .codeRecordSize = 3},
    {Cmd::rgba , .name = "rgba" , .argQuant = 4, .codeRecordSize = 6},
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const size_t CmdInfoArrSize = sizeof(CmdInfoArr) / sizeof(CmdInfoArr[0]);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static_assert((Cmd::CMD_QUANT == (int) CmdInfoArrSize), "You forgot about some Cmd in CmdInfoArr");

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum Registers
{
    ax = 0,
    bx,
    cx,
    dx,
    ex,
    fx,
    REGISTERS_QUANT, // Count
    REGISTERS_NAME_LEN = 2, // in my assebler-standart all registers must have the same name's lenght
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct PushType
{
    unsigned int stk : 1;
    unsigned int reg : 1;
    unsigned int mem : 1;
    unsigned int sum : 1;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct PopType
{
    unsigned int reg : 1;
    unsigned int mem : 1;
    unsigned int sum : 1;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum ArithmeticOperator : int
{
    plus           = Cmd::add , 
    minus          = Cmd::sub ,
    multiplication = Cmd::mul ,
    division       = Cmd::dive,
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum ComparisonOperator : int
{
    above           = Cmd::ja ,
    above_or_equal  = Cmd::jae,
    bellow          = Cmd::jb ,
    bellow_or_equal = Cmd::jbe,
    equal           = Cmd::je ,
    not_equal       = Cmd::jne,
    always_true     = Cmd::jmp,
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct IOfile
{
    const char* ProgrammFile;
    const char* CodeFile;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif //GLOBAL_INCLUDE_HPP
