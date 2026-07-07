#ifndef SEMANTIC_TABLE_H
#define SEMANTIC_TABLE_H
class SemanticTable {
public:
    enum Types {INT = 0, FLO, CHA, STR, BOO};
    enum Operations {SUM = 0, SUB, MUL, DIV, REL, MOD, POT, ROO, AND, OR_};
    enum Status {ERR = -1, WAR, OK_};
    static int const expTable [5][5][10];
    static int const atribTable [5][5];
    static int resultType (int TP1, int TP2, int OP) {
        if(TP1 < 0 || TP2 < 0 || OP < 0) return ERR;
        return (expTable[TP1][TP2][OP]);
    }
    static int atribType (int TP1, int TP2){
        if(TP1 < 0 || TP2 < 0) return ERR;
        return (atribTable[TP1][TP2]);
    }
};
int const SemanticTable::expTable[5][5][10] =
                {/*                INT                  */ /*                 FLOAT               */ /*                 CHAR                */ /*               STRING                */ /*                 BOOL                */
                /*  SUM,SUB,MUL,DIV,REL,MOD,POT,SQT,AND,OR_ , SUM,SUB,MUL,DIV,REL,MOD,POT,SQT,AND,OR_ , SUM,SUB,MUL,DIV,REL,MOD,POT,SQT,AND,OR_ , SUM,SUB,MUL,DIV,REL,MOD,POT,SQT,AND,OR_ , SUM,SUB,MUL,DIV,REL,MOD,POT,SQT,AND,OR_     */
        /*INT   */ {{INT,INT,INT,FLO,BOO,INT,INT,FLO,ERR,ERR},{FLO,FLO,FLO,FLO,BOO,ERR,FLO,FLO,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR}},
        /*FLOAT */ {{FLO,FLO,FLO,FLO,BOO,ERR,FLO,FLO,ERR,ERR},{FLO,FLO,FLO,FLO,BOO,ERR,FLO,FLO,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR}},
        /*CHAR  */ {{ERR,ERR,ERR,ERR,BOO,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,BOO,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,BOO,ERR,ERR,ERR,ERR,ERR},{STR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR}},
        /*STRING*/ {{STR,ERR,ERR,ERR,BOO,ERR,ERR,ERR,ERR,ERR},{STR,ERR,ERR,ERR,BOO,ERR,ERR,ERR,ERR,ERR},{STR,ERR,ERR,ERR,BOO,ERR,ERR,ERR,ERR,ERR},{STR,ERR,ERR,ERR,BOO,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR}},
        /*BOOL  */ {{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR},{ERR,ERR,ERR,ERR,BOO,ERR,ERR,ERR,BOO,BOO}}
};
int const SemanticTable::atribTable[5][5]=
    {     /* INT FLO CHA STR BOO  */
        /*INT*/ {OK_,WAR,ERR,ERR,ERR},
        /*FLO*/ {OK_,OK_,ERR,ERR,ERR},
        /*CHA*/ {ERR,ERR,OK_,ERR,ERR},
        /*STR*/ {ERR,ERR,OK_,OK_,ERR},
        /*BOO*/ {ERR,ERR,ERR,ERR,OK_}
};
#endif