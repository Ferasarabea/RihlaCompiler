//-----------------------------------------------------------
// Dr. Art Hanna
// SPL5 Compiler
// SPL5Compiler.cpp
//-----------------------------------------------------------
#include <iostream>
#include <iomanip>

#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <vector>

using namespace std;

//#define TRACEREADER
//#define TRACESCANNER
#define TRACEPARSER
//#define TRACEIDENTIFIERTABLE
#define TRACECOMPILER

#include "Rihla.h"

/*
========================
Changes to SPL4 compiler
========================
Added tokens
   (pseudo-terminals)
   (  reserved words) FOR TO BY
   (     punctuation) OBRACE CBRACE
   (       operators)

Updated functions
   ParseStatement
   ParseVariable
      
Added functions
   ParseAssertion
   ParseFORStatement

========================
Added ***extras*** of SPL5 language
========================
Added tokens
   (pseudo-terminals) FOR2 CHOOSE ONE ALL WHEN DO2 TIMES
   (  reserved words) 
   (     punctuation) 
   (       operators)

Updated functions
   ParseStatement
   ParseAssignmentStatement
      
Added functions
   ParseCHOOSEStatement
   ParseFOR2Statement
   ParseDOTIMESStatement
End added ***extras*** of SPL4 language
*/

//-----------------------------------------------------------
typedef enum
//-----------------------------------------------------------
{
// pseudo-terminals
   IDENTIFIER,
   INTEGER,
   STRING,
   EOPTOKEN,
   UNKTOKEN,
// reserved words
   PROGRAM,
   END,
   WRITE,
   PRINT,
   SHOW,
   NL,
   LOGICAL_AND,
   LOGICAL_OR,
   ENDL,
   ENDRIHLA,
   OR,
   NOR,
   XOR,
   AND,
   NAND,
   NOT,
   ABS,
   TRUE,
   FALSE,
   VAR,
   INT,
   BOOL,
   CON,
   INPUT,
   IF,
   THEN,
   ELIF,
   ELSE,
   DO,
   WHILE,
   FOR,
   TO,
   BY,
   PROCEDURE,
   IN,
   OUT,
   IO,
   REF,
   CALL,
   RETURN,
   FUNCTION,
   LB,
   UB,
// Added ***extras*** of SPL5 language
   FOR2,
   CHOOSE,
   ONE,
   ALL,
   WHEN,
   DO2,
   TIMES,
// End added ***extras*** of SPL5 language
// punctuation
   COMMA,
   PERIOD,
   OPARENTHESIS,
   CPARENTHESIS,
   COLON,
   COLONEQ,
   OBRACE,
   CBRACE,
   OBRACKET,
   CBRACKET,
// operators
   LT,
   LTEQ,
   EQ,
   GT,
   GTEQ,
   NOTEQ, // <> and !=
   PLUS,
   MINUS,
   MULTIPLY,
   DIVIDE,
   MODULUS,
   POWER,  // ^ and **
   INC,
   DEC
} TOKENTYPE;

//-----------------------------------------------------------
struct TOKENTABLERECORD
//-----------------------------------------------------------
{
   TOKENTYPE type;
   char description[12+1];
   bool isReservedWord;
};

//-----------------------------------------------------------
const TOKENTABLERECORD TOKENTABLE[] =
//-----------------------------------------------------------
{
   { IDENTIFIER  ,"IDENTIFIER"  ,false },
   { INTEGER     ,"INTEGER"     ,false },
   { STRING      ,"STRING"      ,false },
   { EOPTOKEN    ,"EOPTOKEN"    ,false },
   { UNKTOKEN    ,"UNKTOKEN"    ,false },
   { PROGRAM     ,"PROGRAM"     ,true  },
   { NL          ,"NL"          ,true  },
   { WRITE       ,"WRITE"       ,true  },
   { ENDRIHLA    ,"ENDRIHLA"    ,true  },
   { END         ,"END"         ,true  },
   { SHOW        ,"SHOW"        ,true  },
   { PRINT       ,"PRINT"       ,true  },
   { ENDL        ,"ENDL"        ,true  },
   { OR          ,"OR"          ,true  },
   { NOR         ,"NOR"         ,true  },
   { XOR         ,"XOR"         ,true  },
   { LOGICAL_AND ,"&&"          ,true  },
   { LOGICAL_OR  ,"||"          ,true  },
   { AND         ,"AND"         ,true  },
   { NAND        ,"NAND"        ,true  },
   { NOT         ,"NOT"         ,true  },
   { ABS         ,"ABS"         ,true  },
   { TRUE        ,"TRUE"        ,true  },
   { FALSE       ,"FALSE"       ,true  },
   { VAR         ,"VAR"         ,true  },
   { INT         ,"INT"         ,true  },
   { BOOL        ,"BOOL"        ,true  },
   { CON         ,"CON"         ,true  },
   { INPUT       ,"INPUT"       ,true  },
   { IF          ,"IF"          ,true  },
   { THEN        ,"THEN"        ,true  },
   { ELIF        ,"ELIF"        ,true  },
   { ELSE        ,"ELSE"        ,true  },
   { DO          ,"DO"          ,true  },
   { WHILE       ,"WHILE"       ,true  },
   { FOR         ,"FOR"         ,true  },
   { TO          ,"TO"          ,true  },
   { BY          ,"BY"          ,true  },
   { PROCEDURE   ,"PROCEDURE"   ,true  },
   { IN          ,"IN"          ,true  },
   { OUT         ,"OUT"         ,true  },
   { IO          ,"IO"          ,true  },
   { REF         ,"REF"         ,true  },
   { CALL        ,"CALL"        ,true  },
   { RETURN      ,"RETURN"      ,true  },
   { FUNCTION    ,"FUNCTION"    ,true  },
   { LB          ,"LB"          ,true  },
   { UB          ,"UB"          ,true  },
// Added ***extras*** of SPL5 language
   { FOR2        ,"FOR2"        ,true  },
   { CHOOSE      ,"CHOOSE"      ,true  },
   { ONE         ,"ONE"         ,true  },
   { ALL         ,"ALL"         ,true  },
   { WHEN        ,"WHEN"        ,true  },
   { DO2         ,"DO2"         ,true  },
   { TIMES       ,"TIMES"       ,true  },
// End added ***extras*** of SPL5 language
   { COMMA       ,"COMMA"       ,false },
   { PERIOD      ,"PERIOD"      ,false },
   { OPARENTHESIS,"OPARENTHESIS",false },
   { CPARENTHESIS,"CPARENTHESIS",false },
   { COLON       ,"COLON"       ,false },
   { COLONEQ     ,"COLONEQ"     ,false },
   { OBRACE      ,"OBRACE"      ,false },
   { CBRACE      ,"CBRACE"      ,false },
   { OBRACKET    ,"OBRACKET"    ,true },
   { CBRACKET    ,"CBRACKET"    ,true },
   { LT          ,"LT"          ,false },
   { LTEQ        ,"LTEQ"        ,false },
   { EQ          ,"EQ"          ,false },
   { GT          ,"GT"          ,false },
   { GTEQ        ,"GTEQ"        ,false },
   { NOTEQ       ,"NOTEQ"       ,false },
   { PLUS        ,"PLUS"        ,false },
   { MINUS       ,"MINUS"       ,false },
   { MULTIPLY    ,"MULTIPLY"    ,false },
   { DIVIDE      ,"DIVIDE"      ,false },
   { MODULUS     ,"MODULUS"     ,false },
   { POWER       ,"POWER"       ,false },
   { INC         ,"INC"         ,false },
   { DEC         ,"DEC"         ,false }
};

//-----------------------------------------------------------
struct TOKEN
//-----------------------------------------------------------
{
   TOKENTYPE type;
   char lexeme[SOURCELINELENGTH+1];
   int sourceLineNumber;
   int sourceLineIndex;
};

//--------------------------------------------------
// Global variables
//--------------------------------------------------
READER<CALLBACKSUSED> reader(SOURCELINELENGTH,LOOKAHEAD);
LISTER lister(LINESPERPAGE);
// CODEGENERATION
CODE code;
IDENTIFIERTABLE identifierTable(&lister,MAXIMUMIDENTIFIERS);
// ENDCODEGENERATION

#ifdef TRACEPARSER
int level;
#endif

//-----------------------------------------------------------
void EnterModule(const char module[])
//-----------------------------------------------------------
{
#ifdef TRACEPARSER
   char information[SOURCELINELENGTH+1];

   level++;
   sprintf(information,"   %*s>%s",level*2," ",module);
   lister.ListInformationLine(information);
#endif
}

//-----------------------------------------------------------
void ExitModule(const char module[])
//-----------------------------------------------------------
{
#ifdef TRACEPARSER
   char information[SOURCELINELENGTH+1];

   sprintf(information,"   %*s<%s",level*2," ",module);
   lister.ListInformationLine(information);
   level--;
#endif
}

//--------------------------------------------------
void ProcessCompilerError(int sourceLineNumber,int sourceLineIndex,const char errorMessage[])
//--------------------------------------------------
{
   char information[SOURCELINELENGTH+1];

// Use "panic mode" error recovery technique: report error message and terminate compilation!
   sprintf(information,"     At (%4d:%3d) %s",sourceLineNumber,sourceLineIndex,errorMessage);
   lister.ListInformationLine(information);
   lister.ListInformationLine("RIHLA compiler ending with compiler error!\n");
   throw( SPLEXCEPTION("RIHLA compiler ending with compiler error!") );
}

//-----------------------------------------------------------
int main()
//-----------------------------------------------------------
{
   void Callback1(int sourceLineNumber,const char sourceLine[]);
   void Callback2(int sourceLineNumber,const char sourceLine[]);
   void ParseSPLProgram(TOKEN tokens[]);
   void GetNextToken(TOKEN tokens[]);

   char sourceFileName[80+1];
   TOKEN tokens[LOOKAHEAD+1];
   
   cout << "Source filename? "; cin >> sourceFileName;

   try
   {
      lister.OpenFile(sourceFileName);
      code.OpenFile(sourceFileName);

// CODEGENERATION
      code.EmitBeginningCode(sourceFileName);
// ENDCODEGENERATION

      reader.SetLister(&lister);
      reader.AddCallbackFunction(Callback1);
      reader.AddCallbackFunction(Callback2);
      reader.OpenFile(sourceFileName);

   // Fill tokens[] for look-ahead
      for (int i = 0; i <= LOOKAHEAD; i++)
         GetNextToken(tokens);

#ifdef TRACEPARSER
      level = 0;
#endif
   
      ParseSPLProgram(tokens);

// CODEGENERATION
      code.EmitEndingCode();
// ENDCODEGENERATION

   }
   catch (SPLEXCEPTION splException)
   {
      cout << "RIHLA exception: " << splException.GetDescription() << endl;
   }
   lister.ListInformationLine("******* RIHLA compiler ending");
   cout << "RIHLA compiler ending\n";

   system("PAUSE");
   return( 0 );
}

//-----------------------------------------------------------
void ParseSPLProgram(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseDataDefinitions(TOKEN tokens[],IDENTIFIERSCOPE identifierScope);
   void ParsePROCEDUREDefinition(TOKEN tokens[]);
   void ParseFUNCTIONDefinition(TOKEN tokens[]);
   void ParsePROGRAMDefinition(TOKEN tokens[]);
   void GetNextToken(TOKEN tokens[]);

   EnterModule("RIHLAProgram");

   ParseDataDefinitions(tokens,GLOBALSCOPE);

#ifdef TRACECOMPILER
   identifierTable.DisplayTableContents("Contents of identifier table after compilation of global data definitions");
#endif
   while ( (tokens[0].type == PROCEDURE) || (tokens[0].type ==  FUNCTION) )
   {
      switch ( tokens[0].type )
      {
         case PROCEDURE:
            ParsePROCEDUREDefinition(tokens);
            break;
         case FUNCTION:
            ParseFUNCTIONDefinition(tokens);
            break;
      }
   }

   if ( tokens[0].type == PROGRAM )
      ParsePROGRAMDefinition(tokens);
   else
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
                           "Expecting PROGRAM");

   if ( tokens[0].type != EOPTOKEN )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
                           "Expecting end-of-program");

   ExitModule("RIHLAProgram");
}

//-----------------------------------------------------------
void ParseDataDefinitions(TOKEN tokens[],IDENTIFIERSCOPE identifierScope)
//-----------------------------------------------------------
{
   void ParseLBUBRange(TOKEN tokens[],int &LB,int &UB);
   void GetNextToken(TOKEN tokens[]);

   EnterModule("DataDefinitions");

   while ( (tokens[0].type == VAR) || (tokens[0].type == CON) )
   {
      switch ( tokens[0].type )
      {
         case VAR:
            do
            {
               char identifier[MAXIMUMLENGTHIDENTIFIER+1];
               char operand[MAXIMUMLENGTHIDENTIFIER+1];
               char comment[MAXIMUMLENGTHIDENTIFIER+1];
               char reference[MAXIMUMLENGTHIDENTIFIER+1];
               DATATYPE datatype;
               bool isInTable;
               int index;
               int dimensions;
               vector<int> LBs;
               vector<int> UBs;
               int capacity;
      
               GetNextToken(tokens);
         
               if ( tokens[0].type != IDENTIFIER )
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting identifier");
               strcpy(identifier,tokens[0].lexeme);
               GetNextToken(tokens);
         
               if ( tokens[0].type != COLON )
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ':'");
               GetNextToken(tokens);
         
               switch ( tokens[0].type )
               {
                  case INT:
                     datatype = INTTYPE;
                     break;
                  case BOOL:
                     datatype = BOOLTYPE;
                     break;
                  default:
                     ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting INT or BOOL");
               }
               GetNextToken(tokens);
         
               index = identifierTable.GetIndex(identifier,isInTable);
               if ( isInTable && identifierTable.IsInCurrentScope(index) )
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Multiply-defined identifier");

               dimensions = 0;
               
               if ( tokens[0].type == OBRACKET )
               {
                  LBs.clear(); UBs.clear();
                  capacity = 1;
                  do
                  {
                     int LB,UB;
                     
                     GetNextToken(tokens);
                     ParseLBUBRange(tokens,LB,UB);
                     if ( LB > UB )
                        ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"LB > UB in index range");
                     dimensions++;
                     LBs.push_back(LB); UBs.push_back(UB);
                     capacity *= (UB-LB+1);
                  } while ( tokens[0].type == COMMA );
                  if ( tokens[0].type != CBRACKET )
                     ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ']'");
                  GetNextToken(tokens);
               }
               if ( dimensions == 0 )  // scalar variable
               {
                  switch ( identifierScope )
                  {
                     case GLOBALSCOPE:
// CODEGENERATION
                        code.AddRWToStaticData(1,identifier,reference);
// ENDCODEGENERATION
                        identifierTable.AddToTable(identifier,GLOBAL_VARIABLE,datatype,reference);
                        break;
                     case PROGRAMMODULESCOPE:
// CODEGENERATION
                        code.AddRWToStaticData(1,identifier,reference);
// ENDCODEGENERATION
                        identifierTable.AddToTable(identifier,PROGRAMMODULE_VARIABLE,datatype,reference);
                        break;
                     case SUBPROGRAMMODULESCOPE:
// CODEGENERATION
                        sprintf(reference,"FB:0D%d",code.GetFBOffset());
                        code.IncrementFBOffset(1);
// ENDCODEGENERATION
                        identifierTable.AddToTable(identifier,SUBPROGRAMMODULE_VARIABLE,datatype,reference);
                        break;
                  }
               }
               else                    // array variable 
               {
// CODEGENERATION
                  int base;

                  switch ( identifierScope )
                  {
                     case GLOBALSCOPE:
                        sprintf(operand,"0D%d",dimensions);
                        sprintf(comment,"%s at SB:0D%d",identifier,code.GetSBOffset());
                        code.AddDWToStaticData(operand,comment,reference);
                        identifierTable.AddToTable(identifier,GLOBAL_VARIABLE,datatype,reference,dimensions);
                        for (int i = 1; i <= dimensions; i++)
                        {
                           if ( LBs[i-1] < 0 )
                              sprintf(operand,"-0D%d",-LBs[i-1]);
                           else
                              sprintf(operand,"+0D%d",+LBs[i-1]);
                           code.AddDWToStaticData(operand,"",reference);
                           if ( UBs[i-1] < 0 )
                              sprintf(operand,"-0D%d",-UBs[i-1]);
                           else
                              sprintf(operand,"+0D%d",+UBs[i-1]);
                           code.AddDWToStaticData(operand,"",reference);
                        }
                        code.AddRWToStaticData(capacity,"",reference);
                        break;
                     case PROGRAMMODULESCOPE:
                        sprintf(operand,"0D%d",dimensions);
                        sprintf(comment,"%s at SB:0D%d",identifier,code.GetSBOffset());
                        code.AddDWToStaticData(operand,comment,reference);
                        identifierTable.AddToTable(identifier,PROGRAMMODULE_VARIABLE,datatype,reference,dimensions);
                        for (int i = 1; i <= dimensions; i++)
                        {
                           if ( LBs[i-1] < 0 )
                              sprintf(operand,"-0D%d",-LBs[i-1]);
                           else
                              sprintf(operand,"+0D%d",+LBs[i-1]);
                           code.AddDWToStaticData(operand,"",reference);
                           if ( UBs[i-1] < 0 )
                              sprintf(operand,"-0D%d",-UBs[i-1]);
                           else
                              sprintf(operand,"+0D%d",+UBs[i-1]);
                           code.AddDWToStaticData(operand,"",reference);
                        }
                        code.AddRWToStaticData(capacity,"",reference);
                        break;
                     case SUBPROGRAMMODULESCOPE:
                        code.IncrementFBOffset(2*dimensions+capacity);    // not 1+2*dimensions+capacity because 1 word 
                        base = code.GetFBOffset();                        // is already available because of 
                        code.IncrementFBOffset(1);                        // "FBOffset points-to next available word" rule
                        sprintf(reference,"FB:0D%d",base);
                        identifierTable.AddToTable(identifier,SUBPROGRAMMODULE_VARIABLE,datatype,reference,dimensions);

                        sprintf(reference,"FB:0D%d",base);
                        sprintf(operand,"#0D%d",dimensions);
                        sprintf(comment,"initialize array %s at FB:0D%d",identifier,base);
                        code.AddInstructionToInitializeFrameData("PUSH",operand,comment);
                        code.AddInstructionToInitializeFrameData("POP",reference);
                        for (int i = 1; i <= dimensions; i++)
                        {
                           if ( LBs[i-1] < 0 )
                              sprintf(operand,"#-0D%d",-LBs[i-1]);
                           else
                              sprintf(operand,"#+0D%d",+LBs[i-1]);
                           sprintf(reference,"FB:0D%d",base-(2*(i-1)+1));
                           code.AddInstructionToInitializeFrameData("PUSH",operand);
                           code.AddInstructionToInitializeFrameData("POP",reference);

                           if ( UBs[i-1] < 0 )
                              sprintf(operand,"#-0D%d",-UBs[i-1]);
                           else
                              sprintf(operand,"#+0D%d",+UBs[i-1]);
                           sprintf(reference,"FB:0D%d",base-(2*(i-1)+2));
                           code.AddInstructionToInitializeFrameData("PUSH",operand);
                           code.AddInstructionToInitializeFrameData("POP",reference);
                        }
                        break;
                  }
               }
// ENDCODEGENERATION
            } while ( tokens[0].type == COMMA );
            if ( tokens[0].type != PERIOD )
               ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '.'");
            GetNextToken(tokens);
            break;
         case CON:
            do
            {
               char identifier[MAXIMUMLENGTHIDENTIFIER+1];
               char literal[MAXIMUMLENGTHIDENTIFIER+1];
               char operand[MAXIMUMLENGTHIDENTIFIER+1];
               char comment[MAXIMUMLENGTHIDENTIFIER+1];
               char reference[MAXIMUMLENGTHIDENTIFIER+1];
               DATATYPE datatype;
               bool isInTable;
               int index;
      
               GetNextToken(tokens);
         
               if ( tokens[0].type != IDENTIFIER )
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting identifier");
               strcpy(identifier,tokens[0].lexeme);
               GetNextToken(tokens);
         
               if ( tokens[0].type != COLON )
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ':'");
               GetNextToken(tokens);
         
               switch ( tokens[0].type )
               {
                  case INT:
                     datatype = INTTYPE;
                     break;
                  case BOOL:
                     datatype = BOOLTYPE;
                     break;
                  default:
                     ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting INT or BOOL");
               }
               GetNextToken(tokens);
         
               if ( tokens[0].type != COLONEQ )
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ':='");
               GetNextToken(tokens);
         
               if      ( (datatype == INTTYPE) && (tokens[0].type == INTEGER) )
               {
                  strcpy(literal,"0D");
                  strcat(literal,tokens[0].lexeme);
               }
               else if ( ((datatype == BOOLTYPE) && (tokens[0].type ==    TRUE))
                      || ((datatype == BOOLTYPE) && (tokens[0].type ==   FALSE))  )
                 strcpy(literal,tokens[0].lexeme);
               else
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Data type mismatch");
               GetNextToken(tokens);
          
               index = identifierTable.GetIndex(identifier,isInTable);
               if ( isInTable && identifierTable.IsInCurrentScope(index) )
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Multiply-defined identifier");

               switch ( identifierScope )
               {
                  case GLOBALSCOPE:
// CODEGENERATION
                     code.AddDWToStaticData(literal,identifier,reference);
// ENDCODEGENERATION
                     identifierTable.AddToTable(identifier,GLOBAL_CONSTANT,datatype,reference);
                     break;
                  case PROGRAMMODULESCOPE:
// CODEGENERATION
                     code.AddDWToStaticData(literal,identifier,reference);
// ENDCODEGENERATION
                     identifierTable.AddToTable(identifier,PROGRAMMODULE_CONSTANT,datatype,reference);
                     break;
                  case SUBPROGRAMMODULESCOPE:
// CODEGENERATION
                     sprintf(reference,"FB:0D%d",code.GetFBOffset());
                     strcpy(operand,"#"); strcat(operand,literal);
                     sprintf(comment,"initialize constant %s",identifier);
                     code.AddInstructionToInitializeFrameData("PUSH",operand,comment);
                     code.AddInstructionToInitializeFrameData("POP",reference);
                     code.IncrementFBOffset(1);
// ENDCODEGENERATION
                     identifierTable.AddToTable(identifier,SUBPROGRAMMODULE_CONSTANT,datatype,reference);
                     break;
               }
            } while ( tokens[0].type == COMMA );
      
            if ( tokens[0].type != PERIOD )
               ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '.'");
            GetNextToken(tokens);
            break;
       }
   }

   ExitModule("DataDefinitions");
}

//-----------------------------------------------------------
void ParseLBUBRange(TOKEN tokens[],int &LB,int &UB)
//-----------------------------------------------------------
{
   void GetNextToken(TOKEN tokens[]);

   int LBsign,UBsign;

   EnterModule("LBUBRange");

   if      ( tokens[0].type ==  PLUS )
   {
      LBsign = +1;
      GetNextToken(tokens);
   }
   else if ( tokens[0].type == MINUS )
   {
      LBsign = -1;
      GetNextToken(tokens);
   }
   else
      LBsign = +1;
   if ( tokens[0].type != INTEGER )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting integer");
   LB = LBsign*atoi(tokens[0].lexeme);
   GetNextToken(tokens);

   if ( tokens[0].type != COLON )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ':'");
   GetNextToken(tokens);

   if      ( tokens[0].type ==  PLUS )
   {
      UBsign = +1;
      GetNextToken(tokens);
   }
   else if ( tokens[0].type == MINUS )
   {
      UBsign = -1;
      GetNextToken(tokens);
   }
   else
      UBsign = +1;

   if ( tokens[0].type != INTEGER )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting integer");
   UB = UBsign*atoi(tokens[0].lexeme);
   GetNextToken(tokens);

   ExitModule("LBUBRange");
}


//-----------------------------------------------------------
void ParsePROCEDUREDefinition(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseFormalParameter(TOKEN tokens[],IDENTIFIERTYPE &identifierType,int &n);
   void ParseDataDefinitions(TOKEN tokens[],IDENTIFIERSCOPE identifierScope);
   void ParseStatement(TOKEN tokens[]);
   void GetNextToken(TOKEN tokens[]);

   bool isInTable;
   char line[SOURCELINELENGTH+1];
   int index;
   char reference[SOURCELINELENGTH+1];

// n = # formal parameters, m = # words of "save-register" space and locally-defined variables/constants
   int n,m;
   char label[SOURCELINELENGTH+1],operand[SOURCELINELENGTH+1],comment[SOURCELINELENGTH+1];

   EnterModule("PROCEDUREDefinition");

   GetNextToken(tokens);

   if ( tokens[0].type != IDENTIFIER )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting identifier");

   index = identifierTable.GetIndex(tokens[0].lexeme,isInTable);
   if ( isInTable && identifierTable.IsInCurrentScope(index) )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Multiply-defined identifier");

   identifierTable.AddToTable(tokens[0].lexeme,PROCEDURE_SUBPROGRAMMODULE,NOTYPE,tokens[0].lexeme);

// CODEGENERATION
   code.EnterModuleBody(PROCEDURE_SUBPROGRAMMODULE,index);
   code.ResetFrameData();
   code.EmitUnformattedLine("; **** =========");
   sprintf(line,"; **** PROCEDURE module (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);
   code.EmitUnformattedLine("; **** =========");
   code.EmitFormattedLine(tokens[0].lexeme,"EQU","*");
// ENDCODEGENERATION

   identifierTable.EnterNestedStaticScope();

   GetNextToken(tokens);
   n = 0;
   if ( tokens[0].type == OPARENTHESIS )
   {
      do
      {
         IDENTIFIERTYPE parameterIdentifierType;

         GetNextToken(tokens);
         ParseFormalParameter(tokens,parameterIdentifierType,n);
      } while ( tokens[0].type == COMMA );

      if ( tokens[0].type != CPARENTHESIS )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ')'");
      GetNextToken(tokens);
   }

#ifdef TRACECOMPILER
   identifierTable.DisplayTableContents("Contents of identifier table after compilation of PROCEDURE module header");
#endif

// CODEGENERATION  (makes room in frame for caller's saved FB register and the CALL return address)
   code.IncrementFBOffset(2);
// ENDCODEGENERATION

   ParseDataDefinitions(tokens,SUBPROGRAMMODULESCOPE);

#ifdef TRACECOMPILER
   identifierTable.DisplayTableContents("Contents of identifier table after compilation of PROCEDURE local data definitions");
#endif

// CODEGENERATION
   m = code.GetFBOffset()-(n+2);
   code.EmitFormattedLine("","PUSHSP","","set PROCEDURE module FB = SP-on-entry + 2(n+2)");
   sprintf(operand,"#0D%d",2*(n+2));
   sprintf(comment,"n = %d",n);
   code.EmitFormattedLine("","PUSH",operand,comment);
   code.EmitFormattedLine("","ADDI");
   code.EmitFormattedLine("","POPFB");
   code.EmitFormattedLine("","PUSHSP","","PROCEDURE module SP = SP-on-entry + 2m");
   sprintf(operand,"#0D%d",2*m);
   sprintf(comment,"m = %d",m);
   code.EmitFormattedLine("","PUSH",operand,comment);
   code.EmitFormattedLine("","SUBI");
   code.EmitFormattedLine("","POPSP");
   code.EmitUnformattedLine("; statements to initialize frame data (if necessary)");
   code.EmitFrameData();
   sprintf(label,"MODULEBODY%04d",code.LabelSuffix());
   code.EmitFormattedLine("","CALL",label);
   code.EmitFormattedLine("","PUSHFB","","restore caller's SP-on-entry = FB - 2(n+2)");
   sprintf(operand,"#0D%d",2*(n+2));
   code.EmitFormattedLine("","PUSH",operand);
   code.EmitFormattedLine("","SUBI");
   code.EmitFormattedLine("","POPSP");
   code.EmitFormattedLine("","RETURN","","return to caller");
   code.EmitFormattedLine(label,"EQU","*");
   code.EmitUnformattedLine("; statements in body of PROCEDURE module (may include RETURN)");
// ENDCODEGENERATION

   while ( tokens[0].type != END )
      ParseStatement(tokens);

// CODEGENERATION
   code.EmitFormattedLine("","RETURN");
   code.EmitUnformattedLine("; **** =========");
   sprintf(line,"; **** END (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);
   code.EmitUnformattedLine("; **** =========");
   code.ExitModuleBody();
// ENDCODEGENERATION

   identifierTable.ExitNestedStaticScope();

#ifdef TRACECOMPILER
   identifierTable.DisplayTableContents("Contents of identifier table at end of compilation of PROCEDURE module definition");
#endif

   GetNextToken(tokens);

   ExitModule("PROCEDUREDefinition");
}

//-----------------------------------------------------------
void ParseFUNCTIONDefinition(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseFormalParameter(TOKEN tokens[],IDENTIFIERTYPE &identifierType,int &n);
   void ParseDataDefinitions(TOKEN tokens[],IDENTIFIERSCOPE identifierScope);
   void ParseStatement(TOKEN tokens[]);
   void GetNextToken(TOKEN tokens[]);

   bool isInTable;
   DATATYPE datatype;
   char identifier[SOURCELINELENGTH+1];
   char line[SOURCELINELENGTH+1];
   int index;
   char reference[SOURCELINELENGTH+1];

// n = # formal parameters, m = # words of return-value, "save-register" space, and locally-defined variables/constants
   int n,m;
   char label[SOURCELINELENGTH+1],operand[SOURCELINELENGTH+1],comment[SOURCELINELENGTH+1];

   EnterModule("FUNCTIONDefinition");

   GetNextToken(tokens);

   if ( tokens[0].type != IDENTIFIER )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting identifier");

   strcpy(identifier,tokens[0].lexeme);
   index = identifierTable.GetIndex(identifier,isInTable);
   if ( isInTable && identifierTable.IsInCurrentScope(index) )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Multiply-defined identifier");
   GetNextToken(tokens);

   if ( tokens[0].type != COLON )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ':'");
   GetNextToken(tokens);

   switch ( tokens[0].type )
   {
      case INT:
         datatype = INTTYPE;
         break;
      case BOOL:
         datatype = BOOLTYPE;
         break;
      default:
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting INT or BOOL");
   }
   GetNextToken(tokens);

   identifierTable.AddToTable(identifier,FUNCTION_SUBPROGRAMMODULE,datatype,identifier);
   index = identifierTable.GetIndex(identifier,isInTable);

// CODEGENERATION
   code.EnterModuleBody(FUNCTION_SUBPROGRAMMODULE,index);
   code.ResetFrameData();

// Reserve frame-space for FUNCTION return value
   code.IncrementFBOffset(1);

   code.EmitUnformattedLine("; **** =========");
   sprintf(line,"; **** FUNCTION module (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);
   code.EmitUnformattedLine("; **** =========");
   code.EmitFormattedLine(identifier,"EQU","*");
// ENDCODEGENERATION

   identifierTable.EnterNestedStaticScope();

   n = 0;
   if ( tokens[0].type != OPARENTHESIS )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '('");
// Use token look-ahead to make parsing decision
   if ( tokens[1].type != CPARENTHESIS )
   {
      do
      {
         IDENTIFIERTYPE identifierType;

         GetNextToken(tokens);
         ParseFormalParameter(tokens,identifierType,n);

// STATICSEMANTICS
         if ( identifierType != IN_PARAMETER )
            ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"FUNCTION parameter must be IN");
// ENDSTATICSEMANTICS

      } while ( tokens[0].type == COMMA );
   }
   else
      GetNextToken(tokens);
   if ( tokens[0].type != CPARENTHESIS )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ')'");
   GetNextToken(tokens);

#ifdef TRACECOMPILER
   identifierTable.DisplayTableContents("Contents of identifier table after compilation of FUNCTION module header");
#endif

// CODEGENERATION
   code.IncrementFBOffset(2); // makes room in frame for caller's saved FB register and the CALL return address
// ENDCODEGENERATION

   ParseDataDefinitions(tokens,SUBPROGRAMMODULESCOPE);

#ifdef TRACECOMPILER
   identifierTable.DisplayTableContents("Contents of identifier table after compilation of FUNCTION local data definitions");
#endif

// CODEGENERATION
   m = code.GetFBOffset()-(n+3);
   code.EmitFormattedLine("","PUSHSP","","set FUNCTION module FB = SP-on-entry + 2(n+3)");
   sprintf(operand,"#0D%d",2*(n+3));
   sprintf(comment,"n = %d",n);
   code.EmitFormattedLine("","PUSH",operand,comment);
   code.EmitFormattedLine("","ADDI");
   code.EmitFormattedLine("","POPFB");
   code.EmitFormattedLine("","PUSHSP","","FUNCTION module SP = SP-on-entry + 2m");
   sprintf(operand,"#0D%d",2*m);
   sprintf(comment,"m = %d",m);
   code.EmitFormattedLine("","PUSH",operand,comment);
   code.EmitFormattedLine("","SUBI");
   code.EmitFormattedLine("","POPSP");
   code.EmitUnformattedLine("; statements to initialize frame data (if necessary)");
   code.EmitFrameData();
   sprintf(label,"MODULEBODY%04d",code.LabelSuffix());
   code.EmitFormattedLine("","CALL",label);
   code.EmitFormattedLine("","PUSHFB","","restore caller's SP-on-entry = FB - 2(n+3)");
   sprintf(operand,"#0D%d",2*(n+3));
   code.EmitFormattedLine("","PUSH",operand);
   code.EmitFormattedLine("","SUBI");
   code.EmitFormattedLine("","POPSP");
   code.EmitFormattedLine("","RETURN","","return to caller");
   code.EmitFormattedLine(label,"EQU","*");
   code.EmitUnformattedLine("; statements in body of FUNCTION module (*MUST* execute RETURN)");
// ENDCODEGENERATION

    while ( tokens[0].type != END )
      ParseStatement(tokens);

// CODEGENERATION
   sprintf(operand,"#0D%d",tokens[0].sourceLineNumber);
   code.EmitFormattedLine("","PUSH",operand);
   code.EmitFormattedLine("","PUSH","#0D3");
   code.EmitFormattedLine("","JMP","HANDLERUNTIMEERROR");
   code.EmitUnformattedLine("; **** =========");
   sprintf(line,"; **** END (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);
   code.EmitUnformattedLine("; **** =========");
   code.ExitModuleBody();
// ENDCODEGENERATION

   identifierTable.ExitNestedStaticScope();

#ifdef TRACECOMPILER
   identifierTable.DisplayTableContents("Contents of identifier table at end of compilation of FUNCTION module definition");
#endif

   GetNextToken(tokens);

   ExitModule("FUNCTIONDefinition");
}

//-----------------------------------------------------------
void ParseFormalParameter(TOKEN tokens[],IDENTIFIERTYPE &identifierType,int &n)
//-----------------------------------------------------------
{
   void GetNextToken(TOKEN tokens[]);

   char identifier[MAXIMUMLENGTHIDENTIFIER+1],reference[MAXIMUMLENGTHIDENTIFIER+1];
   bool isInTable;
   int index;
   DATATYPE datatype;
   int dimensions;

   EnterModule("FormalParameter");

// CODEGENERATION
   switch ( tokens[0].type )
   {
      case IN:
         identifierType = IN_PARAMETER;
         sprintf(reference,"FB:0D%d",code.GetFBOffset());
         code.IncrementFBOffset(1);
         n += 1;
         GetNextToken(tokens);
         break;
      case OUT:
         identifierType = OUT_PARAMETER;
         code.IncrementFBOffset(1);
         sprintf(reference,"FB:0D%d",code.GetFBOffset());
         code.IncrementFBOffset(1);
         n += 2;
         GetNextToken(tokens);
         break;
      case IO:
         identifierType = IO_PARAMETER;
         code.IncrementFBOffset(1);
         sprintf(reference,"FB:0D%d",code.GetFBOffset());
         code.IncrementFBOffset(1);
         n += 2;
         GetNextToken(tokens);
         break;
      case REF:
         identifierType = REF_PARAMETER;
         sprintf(reference,"@FB:0D%d",code.GetFBOffset());
         code.IncrementFBOffset(1);
         n += 1;
         GetNextToken(tokens);
         break;
      default:
         identifierType = IN_PARAMETER;
         sprintf(reference,"FB:0D%d",code.GetFBOffset());
         code.IncrementFBOffset(1);
         n += 1;
         break;
   }
// ENDCODEGENERATION

   if ( tokens[0].type != IDENTIFIER )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting identifier");
   strcpy(identifier,tokens[0].lexeme);
   GetNextToken(tokens);

   if ( tokens[0].type != COLON )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ':'");
   GetNextToken(tokens);

   switch ( tokens[0].type )
   {
      case INT:
         datatype = INTTYPE;
         break;
      case BOOL:
         datatype = BOOLTYPE;
         break;
      default:
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting INT or BOOL");
   }
   GetNextToken(tokens);

   dimensions = 0;                  
   if ( tokens[0].type == OBRACKET )
   {
      do
      {
         GetNextToken(tokens);
         dimensions++;
      }
      while ( tokens[0].type == COMMA );

      if ( tokens[0].type != CBRACKET )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ']'");

      GetNextToken(tokens);
   }

   if ( (dimensions > 0) && (identifierType != REF_PARAMETER) )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Array must be REF parameter");

   index = identifierTable.GetIndex(identifier,isInTable);
   if ( isInTable && identifierTable.IsInCurrentScope(index) )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Multiply-defined identifier");

   identifierTable.AddToTable(identifier,identifierType,datatype,reference,dimensions);

   ExitModule("FormalParameter");
}

//-----------------------------------------------------------
void ParsePROGRAMDefinition(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseDataDefinitions(TOKEN tokens[],IDENTIFIERSCOPE identifierScope);
   void ParseStatement(TOKEN tokens[]);
   void GetNextToken(TOKEN tokens[]);

   char line[SOURCELINELENGTH+1];
   char label[SOURCELINELENGTH+1];
   char reference[SOURCELINELENGTH+1];

   EnterModule("PROGRAMDefinition");

// CODEGENERATION
   code.EmitUnformattedLine("; **** =========");
   sprintf(line,"; **** PROGRAM module (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);
   code.EmitUnformattedLine("; **** =========");
   code.EmitFormattedLine("PROGRAMMAIN","EQU"  ,"*");
   code.EmitFormattedLine("","PUSH" ,"#RUNTIMESTACK","set SP");
   code.EmitFormattedLine("","POPSP");
   code.EmitFormattedLine("","PUSHA","STATICDATA","set SB");
   code.EmitFormattedLine("","POPSB");
   code.EmitFormattedLine("","PUSH","#HEAPBASE","initialize heap");
   code.EmitFormattedLine("","PUSH","#HEAPSIZE");
   code.EmitFormattedLine("","SVC","#SVC_INITIALIZE_HEAP");
   sprintf(label,"PROGRAMBODY%04d",code.LabelSuffix());
   code.EmitFormattedLine("","CALL",label);
   code.AddDSToStaticData("Normal program termination","",reference);
   code.EmitFormattedLine("","PUSHA",reference);
   code.EmitFormattedLine("","SVC","#SVC_WRITE_STRING");
   code.EmitFormattedLine("","SVC","#SVC_WRITE_ENDL");
   code.EmitFormattedLine("","PUSH","#0D0","terminate with status = 0");
   code.EmitFormattedLine("","SVC" ,"#SVC_TERMINATE");
   code.EmitFormattedLine(label,"EQU","*");
// ENDCODEGENERATION

   GetNextToken(tokens);

   identifierTable.EnterNestedStaticScope();
   ParseDataDefinitions(tokens,PROGRAMMODULESCOPE);

  while (tokens[0].type != END && tokens[0].type != ENDRIHLA)
    ParseStatement(tokens);


// CODEGENERATION
   code.EmitFormattedLine("","RETURN");
   code.EmitUnformattedLine("; **** =========");
   sprintf(line,"; **** END (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);
   code.EmitUnformattedLine("; **** =========");
// ENDCODEGENERATION

#ifdef TRACECOMPILER
   identifierTable.DisplayTableContents("Contents of identifier table at end of compilation of PROGRAM module definition");
#endif

   identifierTable.ExitNestedStaticScope();

   GetNextToken(tokens);

   ExitModule("PROGRAMDefinition");
}
//-----------------------------------------------------------
void ParseSHOWStatement(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseExpression(TOKEN tokens[], DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   char line[SOURCELINELENGTH+1];
   DATATYPE datatype;

   EnterModule("SHOWStatement");

   sprintf(line, "; **** SHOW statement (%4d)", tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   do
   {
      GetNextToken(tokens);

      switch (tokens[0].type)
      {
         case STRING:
            {
               char reference[SOURCELINELENGTH+1];
               code.AddDSToStaticData(tokens[0].lexeme, "", reference);
               code.EmitFormattedLine("", "PUSHA", reference);
               code.EmitFormattedLine("", "SVC", "#SVC_WRITE_STRING");
            }
            GetNextToken(tokens);
            break;

         case ENDL:
         case NL:
            code.EmitFormattedLine("", "SVC", "#SVC_WRITE_ENDL");
            GetNextToken(tokens);
            break;

         default:
            ParseExpression(tokens, datatype);
            switch (datatype)
            {
               case INTTYPE:
                  code.EmitFormattedLine("", "SVC", "#SVC_WRITE_INTEGER");
                  break;
               case BOOLTYPE:
                  code.EmitFormattedLine("", "SVC", "#SVC_WRITE_BOOLEAN");
                  break;
            }
      }
   } while (tokens[0].type == COMMA);

   if (tokens[0].type != PERIOD)
      ProcessCompilerError(tokens[0].sourceLineNumber, tokens[0].sourceLineIndex, "Expecting '.'");

   GetNextToken(tokens);

   ExitModule("SHOWStatement");
}
//-----------------------------------------------------------
void ParseWRITEStatement(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseVariable(TOKEN tokens[],bool asLValue,DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   char reference[SOURCELINELENGTH+1];
   char line[SOURCELINELENGTH+1];
   DATATYPE datatype;

   EnterModule("WRITEStatement");

   sprintf(line,"; **** WRITE statement (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   GetNextToken(tokens);

   if ( tokens[0].type == STRING )
   {

// CODEGENERATION
      code.AddDSToStaticData(tokens[0].lexeme,"",reference);
      code.EmitFormattedLine("","PUSHA",reference);
      code.EmitFormattedLine("","SVC","#SVC_WRITE_STRING");
// ENDCODEGENERATION

      GetNextToken(tokens);
   }

   ParseVariable(tokens,true,datatype);

// CODEGENERATION
   switch ( datatype )
   {
      case INTTYPE:
         code.EmitFormattedLine("","SVC","#SVC_READ_INTEGER");
         break;
      case BOOLTYPE:
         code.EmitFormattedLine("","SVC","#SVC_READ_BOOLEAN");
         break;
   }
   code.EmitFormattedLine("","POP","@SP:0D1");
   code.EmitFormattedLine("","DISCARD","#0D1");
// ENDCODEGENERATION

   if ( tokens[0].type != PERIOD )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '.'");

   GetNextToken(tokens);

   ExitModule("WRITEStatement");
}

//-----------------------------------------------------------
void ParseStatement(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseAssertion(TOKEN tokens[]);
   void ParsePRINTStatement(TOKEN tokens[]);
   void ParseINPUTStatement(TOKEN tokens[]);
   void ParseWRITEStatement(TOKEN tokens[]);
   void ParseAssignmentStatement(TOKEN tokens[],const bool requirePERIOD=true);
   void ParseIFStatement(TOKEN tokens[]);
   void ParseDOWHILEStatement(TOKEN tokens[]);
   void ParseFORStatement(TOKEN tokens[]);
   void ParseFOR2Statement(TOKEN tokens[]);
   void ParseCHOOSEStatement(TOKEN tokens[]);
   void ParseCALLStatement(TOKEN tokens[]);
   void ParseRETURNStatement(TOKEN tokens[]);
   void ParseDOTIMESStatement(TOKEN tokens[]);
   void GetNextToken(TOKEN tokens[]);

   EnterModule("Statement");

   while ( tokens[0].type == OBRACE )
      ParseAssertion(tokens);
   switch ( tokens[0].type )
   {
      case PRINT:
         ParsePRINTStatement(tokens);
         break;
      case SHOW:
      	 ParseSHOWStatement(tokens);
      	 break;
      case INPUT:
         ParseINPUTStatement(tokens);
         break;
      case WRITE:
      	 ParseWRITEStatement(tokens);
      	 break;
      case IDENTIFIER:
         ParseAssignmentStatement(tokens);
         break;
      case IF:
         ParseIFStatement(tokens);
         break;
      case DO:
         ParseDOWHILEStatement(tokens);
         break;
      case FOR:
         ParseFORStatement(tokens);
         break;
// Added ***extras*** of SPL5 language
      case FOR2:
         ParseFOR2Statement(tokens);
         break;
      case CHOOSE:
         ParseCHOOSEStatement(tokens);
         break;
      case DO2:
         ParseDOTIMESStatement(tokens);
         break;
      case CALL:
         ParseCALLStatement(tokens);
         break;
      case RETURN:
         ParseRETURNStatement(tokens);
         break;
// End added ***extras*** of SPL5 language
      default:
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
                              "Expecting beginning-of-statement");
         break;
   }
   while ( tokens[0].type == OBRACE )
      ParseAssertion(tokens);

   ExitModule("Statement");
}

//-----------------------------------------------------------
void ParseAssertion(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseExpression(TOKEN tokens[],DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   char line[SOURCELINELENGTH+1];
   DATATYPE datatype;

   EnterModule("Assertion");

   sprintf(line,"; **** %4d: { assertion }",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   GetNextToken(tokens);

   ParseExpression(tokens,datatype);

// STATICSEMANTICS
   if ( datatype != BOOLTYPE )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting boolean expression");
// ENDSTATICSEMANTICS

// CODEGENERATION
/*
      SETT
      JMPT      E????
      PUSH      #0D(sourceLineNumber)
      PUSH      #0D1
      JMP       HANDLERUNTIMEERROR
E???? EQU       *
      DISCARD   #0D1
*/
   char Elabel[SOURCELINELENGTH+1],operand[SOURCELINELENGTH+1];

   code.EmitFormattedLine("","SETT");
   sprintf(Elabel,"E%04d",code.LabelSuffix());
   code.EmitFormattedLine("","JMPT",Elabel);
   sprintf(operand,"#0D%d",tokens[0].sourceLineNumber);
   code.EmitFormattedLine("","PUSH",operand);
   code.EmitFormattedLine("","PUSH","#0D1");
   code.EmitFormattedLine("","JMP","HANDLERUNTIMEERROR");
   code.EmitFormattedLine(Elabel,"EQU","*");
   code.EmitFormattedLine("","DISCARD","#0D1");
// ENDCODEGENERATION

   if ( tokens[0].type != CBRACE )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting }");

   GetNextToken(tokens);

   ExitModule("Assertion");
}

//-----------------------------------------------------------
void ParsePRINTStatement(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseExpression(TOKEN tokens[],DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   char line[SOURCELINELENGTH+1];
   DATATYPE datatype;

   EnterModule("PRINTStatement");

   sprintf(line,"; **** PRINT statement (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   do
   {
      GetNextToken(tokens);

      switch ( tokens[0].type )
      {
         case STRING:

// CODEGENERATION
            char reference[SOURCELINELENGTH+1];

            code.AddDSToStaticData(tokens[0].lexeme,"",reference);
            code.EmitFormattedLine("","PUSHA",reference);
            code.EmitFormattedLine("","SVC","#SVC_WRITE_STRING");
// ENDCODEGENERATION

            GetNextToken(tokens);
            break;
         case ENDL:

// CODEGENERATION
            code.EmitFormattedLine("","SVC","#SVC_WRITE_ENDL");
// ENDCODEGENERATION

            GetNextToken(tokens);
            break;
        case NL:
             code.EmitFormattedLine("","SVC","#SVC_WRITE_ENDL");  // Output newline for NL
             GetNextToken(tokens);
    break;

         default:
         {
            ParseExpression(tokens,datatype);

// CODEGENERATION
            switch ( datatype )
            {
               case INTTYPE:
                  code.EmitFormattedLine("","SVC","#SVC_WRITE_INTEGER");
                  break;
               case BOOLTYPE:
                  code.EmitFormattedLine("","SVC","#SVC_WRITE_BOOLEAN");
                  break;
            }
// ENDCODEGENERATION

         }
      }
   } while ( tokens[0].type == COMMA );

   if ( tokens[0].type != PERIOD )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
                           "Expecting '.'");

   GetNextToken(tokens);

   ExitModule("PRINTStatement");
}

//-----------------------------------------------------------
void ParseINPUTStatement(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseVariable(TOKEN tokens[],bool asLValue,DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   char reference[SOURCELINELENGTH+1];
   char line[SOURCELINELENGTH+1];
   DATATYPE datatype;

   EnterModule("INPUTStatement");

   sprintf(line,"; **** INPUT statement (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   GetNextToken(tokens);

   if ( tokens[0].type == STRING )
   {

// CODEGENERATION
      code.AddDSToStaticData(tokens[0].lexeme,"",reference);
      code.EmitFormattedLine("","PUSHA",reference);
      code.EmitFormattedLine("","SVC","#SVC_WRITE_STRING");
// ENDCODEGENERATION

      GetNextToken(tokens);
   }

   ParseVariable(tokens,true,datatype);

// CODEGENERATION
   switch ( datatype )
   {
      case INTTYPE:
         code.EmitFormattedLine("","SVC","#SVC_READ_INTEGER");
         break;
      case BOOLTYPE:
         code.EmitFormattedLine("","SVC","#SVC_READ_BOOLEAN");
         break;
   }
   code.EmitFormattedLine("","POP","@SP:0D1");
   code.EmitFormattedLine("","DISCARD","#0D1");
// ENDCODEGENERATION

   if ( tokens[0].type != PERIOD )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '.'");

   GetNextToken(tokens);

   ExitModule("INPUTStatement");
}

//-----------------------------------------------------------
// Added ***extras*** of SPL5 language
void ParseAssignmentStatement(TOKEN tokens[],const bool requirePERIOD=true)
// End added ***extras*** of SPL5 language
//-----------------------------------------------------------
{
   void ParseVariable(TOKEN tokens[],bool asLValue,DATATYPE &datatype);
   void ParseExpression(TOKEN tokens[],DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   char line[SOURCELINELENGTH+1];
   DATATYPE datatypeLHS,datatypeRHS;
   int n;

   EnterModule("AssignmentStatement");

   sprintf(line,"; **** assignment statement (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   ParseVariable(tokens,true,datatypeLHS);
   n = 1;

   while ( tokens[0].type == COMMA )
   {
      DATATYPE datatype;

      GetNextToken(tokens);
      ParseVariable(tokens,true,datatype);
      n++;

      if ( datatype != datatypeLHS )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Mixed-mode variables not allowed");
   }
   if ( tokens[0].type != COLONEQ )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ':='");
   GetNextToken(tokens);

   ParseExpression(tokens,datatypeRHS);

   if ( datatypeLHS != datatypeRHS )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Data type mismatch");

// CODEGENERATION
   for (int i = 1; i <= n; i++)
   {
      code.EmitFormattedLine("","MAKEDUP");
      code.EmitFormattedLine("","POP","@SP:0D2");
      code.EmitFormattedLine("","SWAP");
      code.EmitFormattedLine("","DISCARD","#0D1");
   }
   code.EmitFormattedLine("","DISCARD","#0D1");
// ENDCODEGENERATION

// Added ***extras*** of SPL5 language
   if ( requirePERIOD )
   {
      if ( tokens[0].type != PERIOD )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '.'");
      GetNextToken(tokens);
   }
// End added ***extras*** of SPL5 language

   ExitModule("AssignmentStatement");
}

//-----------------------------------------------------------
void ParseIFStatement(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseExpression(TOKEN tokens[],DATATYPE &datatype);
   void ParseStatement(TOKEN tokens[]);
   void GetNextToken(TOKEN tokens[]);

   char line[SOURCELINELENGTH+1];
   char Ilabel[SOURCELINELENGTH+1],Elabel[SOURCELINELENGTH+1];
   DATATYPE datatype;

   EnterModule("IFStatement");

   sprintf(line,"; **** IF statement (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   GetNextToken(tokens);

   if ( tokens[0].type != OPARENTHESIS )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '('");
   GetNextToken(tokens);
   ParseExpression(tokens,datatype);
   if ( tokens[0].type != CPARENTHESIS )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ')'");
   GetNextToken(tokens);
   if ( tokens[0].type != THEN )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting THEN");
   GetNextToken(tokens);

   if ( datatype != BOOLTYPE )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting boolean expression");

// CODEGENERATION
/* 
   Plan for the generalized IF statement with n ELIFs and 1 ELSE (*Note* n
      can be 0 and the ELSE may be missing and the plan still "works.")

   ...expression...           ; boolean expression on top-of-stack
      SETT
      DISCARD   #0D1
      JMPNT     I???1
   ...statements...
      JMP       E????
I???1 EQU       *             ; 1st ELIF clause
   ...expression...
      SETT
      DISCARD   #0D1
      JMPNT     I???2
   ...statements...
      JMP       E????
      .
      .
I???n EQU       *             ; nth ELIF clause
   ...expression...
      SETT
      DISCARD   #0D1
      JMPNT     I????
   ...statements...
      JMP       E????
I???? EQU       *             ; ELSE clause
   ...statements...
E???? EQU       *
*/
   sprintf(Elabel,"E%04d",code.LabelSuffix());
   code.EmitFormattedLine("","SETT");
   code.EmitFormattedLine("","DISCARD","#0D1");
   sprintf(Ilabel,"I%04d",code.LabelSuffix());
   code.EmitFormattedLine("","JMPNT",Ilabel);
// ENDCODEGENERATION

   while ( (tokens[0].type != ELIF) && 
           (tokens[0].type != ELSE) && 
           (tokens[0].type !=  END) )
      ParseStatement(tokens);

// CODEGENERATION
   code.EmitFormattedLine("","JMP",Elabel);
   code.EmitFormattedLine(Ilabel,"EQU","*");
// ENDCODEGENERATION

   while ( tokens[0].type == ELIF )
   {
      GetNextToken(tokens);
      if ( tokens[0].type != OPARENTHESIS )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '('");
      GetNextToken(tokens);
      ParseExpression(tokens,datatype);
      if ( tokens[0].type != CPARENTHESIS )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ')'");
      GetNextToken(tokens);
      if ( tokens[0].type != THEN )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting THEN");
      GetNextToken(tokens);

      if ( datatype != BOOLTYPE )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting boolean expression");

// CODEGENERATION
      code.EmitFormattedLine("","SETT");
      code.EmitFormattedLine("","DISCARD","#0D1");
      sprintf(Ilabel,"I%04d",code.LabelSuffix());
      code.EmitFormattedLine("","JMPNT",Ilabel);
// ENDCODEGENERATION

      while ( (tokens[0].type != ELIF) && 
              (tokens[0].type != ELSE) && 
              (tokens[0].type !=  END) )
         ParseStatement(tokens);

// CODEGENERATION
      code.EmitFormattedLine("","JMP",Elabel);
      code.EmitFormattedLine(Ilabel,"EQU","*");
// ENDCODEGENERATION

   }
   if ( tokens[0].type == ELSE )
   {
      GetNextToken(tokens);
      while ( tokens[0].type != END )
         ParseStatement(tokens);
   }

   GetNextToken(tokens);

// CODEGENERATION
      code.EmitFormattedLine(Elabel,"EQU","*");
// ENDCODEGENERATION

   ExitModule("IFStatement");
}

//-----------------------------------------------------------
void ParseDOWHILEStatement(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseExpression(TOKEN tokens[],DATATYPE &datatype);
   void ParseStatement(TOKEN tokens[]);
   void GetNextToken(TOKEN tokens[]);

   char line[SOURCELINELENGTH+1];
   char Dlabel[SOURCELINELENGTH+1],Elabel[SOURCELINELENGTH+1];
   DATATYPE datatype;

   EnterModule("DOWHILEStatement");

   sprintf(line,"; **** DO-WHILE statement (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   GetNextToken(tokens);

// CODEGENERATION
/*
D???? EQU       *
   ...statements...
   ...expression...
      SETT
      DISCARD   #0D1
      JMPNT     E????
   ...statements...
      JMP       D????
E???? EQU       *
*/

   sprintf(Dlabel,"D%04d",code.LabelSuffix());
   sprintf(Elabel,"E%04d",code.LabelSuffix());
   code.EmitFormattedLine(Dlabel,"EQU","*");
// ENDCODEGENERATION

   while ( tokens[0].type != WHILE )
      ParseStatement(tokens);
   GetNextToken(tokens);
   if ( tokens[0].type != OPARENTHESIS )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '('");
   GetNextToken(tokens);
   ParseExpression(tokens,datatype);
   if ( tokens[0].type != CPARENTHESIS )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ')'");
   GetNextToken(tokens);

   if ( datatype != BOOLTYPE )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting boolean expression");

// CODEGENERATION
   code.EmitFormattedLine("","SETT");
   code.EmitFormattedLine("","DISCARD","#0D1");
   code.EmitFormattedLine("","JMPNT",Elabel);
// ENDCODEGENERATION

   while ( tokens[0].type != END )
      ParseStatement(tokens);

   GetNextToken(tokens);

// CODEGENERATION
   code.EmitFormattedLine("","JMP",Dlabel);
   code.EmitFormattedLine(Elabel,"EQU","*");
// ENDCODEGENERATION

   ExitModule("DOWHILEStatement");
}

//-----------------------------------------------------------
void ParseFORStatement(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseVariable(TOKEN tokens[],bool asLValue,DATATYPE &datatype);
   void ParseExpression(TOKEN tokens[],DATATYPE &datatype);
   void ParseStatement(TOKEN tokens[]);
   void GetNextToken(TOKEN tokens[]);

   char line[SOURCELINELENGTH+1];
   char Dlabel[SOURCELINELENGTH+1],Llabel[SOURCELINELENGTH+1],
        Clabel[SOURCELINELENGTH+1],Elabel[SOURCELINELENGTH+1];
   char operand[SOURCELINELENGTH+1];
   DATATYPE datatype;

   EnterModule("FORStatement");

   sprintf(line,"; **** FOR statement (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   GetNextToken(tokens);

   ParseVariable(tokens,true,datatype);

   if ( datatype != INTTYPE )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting integer variable");

/*
; v := e1
   ...v...                    ; &v = run-time stack (bottom to top)
   ...e1...                   ; &v,e1
      POP       @SP:0D1       ; &v := e1
   ...e2...                   ; &v,e2
   ...e3...                   ; &v,e2,e3
      SETNZPI
; if ( e3 = 0 ) then
      JMPNZ     D????
      PUSH      #0D(current line number)
      PUSH      #0D2
      JMP       HANDLERUNTIMEERROR
D???? SETNZPI
; else if ( e3 > 0 ) then
      JMPN      L????
      SWAP                    ; &v,e3,e2
      MAKEDUP                 ; &v,e3,e2,e2
      PUSH      @SP:0D3       ; &v,e3,e2,e2,v 
      SWAP                    ; &v,e3,e2,v,e2
;    if ( v <= e2 ) continue else end
      CMPI                    ; &v,e3,e2 (set LEG)
      JMPLE     C????
      JMP       E????  
; else ( e3 < 0 )
L???? SWAP                    ; &v,e3,e2
      MAKEDUP                 ; &v,e3,e2,e2
      PUSH      @SP:0D3       ; &v,e3,e2,e2,v 
      SWAP                    ; &v,e3,e2,v,e2
;    if ( v >= e2 ) continue else end
      CMPI                    ; &v,e3,e2 (set LEG)
      JMPGE     C????
      JMP       E????  
; endif
C???? EQU       *
   ...statements...
      SWAP                    ; &v,e2,e3
      MAKEDUP                 ; &v,e2,e3,e3
; v := e3+v
      PUSH      @SP:0D3       ; &v,e2,e3,e3,v
      ADDI                    ; &v,e2,e3,(e3+v)
      POP       @SP:0D3       ; &v,e2,e3
      JMP       D????
E???? DISCARD   #0D3          ; now run-time stack is empty
*/

   if ( tokens[0].type != COLONEQ )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ':='");
   GetNextToken(tokens);

   ParseExpression(tokens,datatype);
   if ( datatype != INTTYPE )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting integer data type");

// CODEGENERATION
   code.EmitFormattedLine("","POP","@SP:0D1");
// ENDCODEGENERATION

   if ( tokens[0].type != TO )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting TO");
   GetNextToken(tokens);

   ParseExpression(tokens,datatype);
   if ( datatype != INTTYPE )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting integer data type");

   if ( tokens[0].type == BY )
   {
      GetNextToken(tokens);

      ParseExpression(tokens,datatype);
      if ( datatype != INTTYPE )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting integer data type");
   }
   else
   {

// CODEGENERATION
      code.EmitFormattedLine("","PUSH","#0D1");
// ENDCODEGENERATION

   }

// CODEGENERATION
   sprintf(Dlabel,"D%04d",code.LabelSuffix());
   sprintf(Llabel,"L%04d",code.LabelSuffix());
   sprintf(Clabel,"C%04d",code.LabelSuffix());
   sprintf(Elabel,"E%04d",code.LabelSuffix());

   code.EmitFormattedLine("","SETNZPI");
   code.EmitFormattedLine("","JMPNZ",Dlabel);
   sprintf(operand,"#0D%d",tokens[0].sourceLineNumber);
   code.EmitFormattedLine("","PUSH",operand);
   code.EmitFormattedLine("","PUSH","#0D2");
   code.EmitFormattedLine("","JMP","HANDLERUNTIMEERROR");

   code.EmitFormattedLine(Dlabel,"SETNZPI");
   code.EmitFormattedLine("","JMPN",Llabel);
   code.EmitFormattedLine("","SWAP");
   code.EmitFormattedLine("","MAKEDUP");
   code.EmitFormattedLine("","PUSH","@SP:0D3");
   code.EmitFormattedLine("","SWAP");
   code.EmitFormattedLine("","CMPI");
   code.EmitFormattedLine("","JMPLE",Clabel);
   code.EmitFormattedLine("","JMP",Elabel);
   code.EmitFormattedLine(Llabel,"SWAP");
   code.EmitFormattedLine("","MAKEDUP");
   code.EmitFormattedLine("","PUSH","@SP:0D3");
   code.EmitFormattedLine("","SWAP");
   code.EmitFormattedLine("","CMPI");
   code.EmitFormattedLine("","JMPGE",Clabel);
   code.EmitFormattedLine("","JMP",Elabel);
   code.EmitFormattedLine(Clabel,"EQU","*");
// ENDCODEGENERATION

   while ( tokens[0].type != END )
      ParseStatement(tokens);

   GetNextToken(tokens);

// CODEGENERATION
   code.EmitFormattedLine("","SWAP");
   code.EmitFormattedLine("","MAKEDUP");
   code.EmitFormattedLine("","PUSH","@SP:0D3");
   code.EmitFormattedLine("","ADDI");
   code.EmitFormattedLine("","POP","@SP:0D3");
   code.EmitFormattedLine("","JMP",Dlabel);
   code.EmitFormattedLine(Elabel,"DISCARD","#0D3");
// ENDCODEGENERATION

   ExitModule("FORStatement");
}

//-----------------------------------------------------------
void ParseCALLStatement(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseVariable(TOKEN tokens[],bool asLValue,DATATYPE &datatype);
   void ParseExpression(TOKEN tokens[],DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   char line[SOURCELINELENGTH+1];
   bool isInTable;
   int index,parameters;

   EnterModule("CALLStatement");

   sprintf(line,"; **** CALL statement (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   GetNextToken(tokens);

   if ( tokens[0].type != IDENTIFIER )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting identifier");

// STATICSEMANTICS
   index = identifierTable.GetIndex(tokens[0].lexeme,isInTable);
   if ( !isInTable )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Undefined identifier");
   if ( identifierTable.GetType(index) != PROCEDURE_SUBPROGRAMMODULE )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting PROCEDURE identifier");
// ENDSTATICSEMANTICS

   GetNextToken(tokens);
   parameters = 0;
   if ( tokens[0].type == OPARENTHESIS )
   {
      DATATYPE expressionDatatype,variableDatatype;

      do
      {
         GetNextToken(tokens);
         parameters++;

// CODEGENERATION   
// STATICSEMANTICS
         switch ( identifierTable.GetType(index+parameters) )
         {
            case IN_PARAMETER:
               ParseExpression(tokens,expressionDatatype);
               if ( expressionDatatype != identifierTable.GetDatatype(index+parameters) )
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
                     "Actual parameter data type does not match formal parameter data type");
               break;
            case OUT_PARAMETER:
               ParseVariable(tokens,true,variableDatatype);
               if ( variableDatatype != identifierTable.GetDatatype(index+parameters) )
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
                     "Actual parameter data type does not match formal parameter data type");
               code.EmitFormattedLine("","PUSH","#0X0000");
               break;
            case IO_PARAMETER:
               ParseVariable(tokens,true,variableDatatype);
               if ( variableDatatype != identifierTable.GetDatatype(index+parameters) )
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
                     "Actual parameter data type does not match formal parameter data type");
               code.EmitFormattedLine("","PUSH","@SP:0D0");
               break;
            case REF_PARAMETER:
               if ( identifierTable.GetDimensions(index+parameters) == 0 )
               {
                  ParseVariable(tokens,true,variableDatatype);
                  if ( variableDatatype != identifierTable.GetDatatype(index+parameters) )
                     ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
                        "Actual parameter data type does not match formal parameter data type");
               }
               else
               {
                  int index2;
                  
                  if ( tokens[0].type != IDENTIFIER )
                     ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting identifier");
                  index2 = identifierTable.GetIndex(tokens[0].lexeme,isInTable);
                  if ( !isInTable )
                     ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Undefined identifier");
                  if ( identifierTable.GetDatatype(index2) != identifierTable.GetDatatype(index+parameters) )
                     ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
                        "Actual parameter data type does not match formal parameter data type");
                  if ( identifierTable.GetDimensions(index2) != identifierTable.GetDimensions(index+parameters) )
                     ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
                        "Actual array parameter dimensions does not match formal array parameter dimensions");
                  code.EmitFormattedLine("","PUSHA",identifierTable.GetReference(index2));
                  GetNextToken(tokens);
               }
               break;
         }
// ENDSTATICSEMANTICS
// ENDCODEGENERATION
      } while ( tokens[0].type == COMMA );

      if ( tokens[0].type != CPARENTHESIS )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting )");

      GetNextToken(tokens);
   }
                     
// STATICSEMANTICS
   if ( identifierTable.GetCountOfFormalParameters(index) != parameters )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
         "Number of actual parameters does not match number of formal parameters");
// ENDSTATICSEMANTICS

// CODEGENERATION
   code.EmitFormattedLine("","PUSHFB");
   code.EmitFormattedLine("","CALL",identifierTable.GetReference(index));
   code.EmitFormattedLine("","POPFB");
   for (parameters = identifierTable.GetCountOfFormalParameters(index); parameters >= 1; parameters--)
   {
      switch ( identifierTable.GetType(index+parameters) )
      {
         case IN_PARAMETER:
            code.EmitFormattedLine("","DISCARD","#0D1");
            break;
         case OUT_PARAMETER:
            code.EmitFormattedLine("","POP","@SP:0D1");
            code.EmitFormattedLine("","DISCARD","#0D1");
            break;
         case IO_PARAMETER:
            code.EmitFormattedLine("","POP","@SP:0D1");
            code.EmitFormattedLine("","DISCARD","#0D1");
            break;
         case REF_PARAMETER:
            code.EmitFormattedLine("","DISCARD","#0D1");
            break;
      }
   }
// ENDCODEGENERATION

   if ( tokens[0].type != PERIOD )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '.'");

   GetNextToken(tokens);

   ExitModule("CALLStatement");
}


//-----------------------------------------------------------
void ParseRETURNStatement(TOKEN tokens[])
//-----------------------------------------------------------
{
   void ParseExpression(TOKEN tokens[],DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   char line[SOURCELINELENGTH+1];

   EnterModule("RETURNStatement");

   sprintf(line,"; **** RETURN statement (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   GetNextToken(tokens);

// STATICSEMANTICS
   if      ( code.IsInModuleBody(PROCEDURE_SUBPROGRAMMODULE) )
// CODEGENERATION
      code.EmitFormattedLine("","RETURN");
// ENDCODEGENERATION
   else if ( code.IsInModuleBody( FUNCTION_SUBPROGRAMMODULE) )
   {
      DATATYPE datatype;

      if ( tokens[0].type != OPARENTHESIS )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '('");
      GetNextToken(tokens);
   
      ParseExpression(tokens,datatype);

      if ( datatype != identifierTable.GetDatatype(code.GetModuleIdentifierIndex()) )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
            "RETURN expression data type must match FUNCTION data type");
   
// CODEGENERATION
      code.EmitFormattedLine("","POP","FB:0D0","pop RETURN expression into function return value");
      code.EmitFormattedLine("","RETURN");
// ENDCODEGENERATION

      if ( tokens[0].type != CPARENTHESIS )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ')'");
      GetNextToken(tokens);
   }
   else 
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
         "RETURN only allowed in PROCEDURE or FUNCTION module body");
// ENDSTATICSEMANTICS

   if ( tokens[0].type != PERIOD )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting .");

   GetNextToken(tokens);

   ExitModule("RETURNStatement");
}

//-----------------------------------------------------------
// Added ***extras*** of SPL5 language
//-----------------------------------------------------------
void ParseFOR2Statement(TOKEN tokens[])
//-----------------------------------------------------------
{
/*
||-----------------------------------------------------------
|| <FOR2Statement> ::= FOR2 ( <assignmentStatement> , <expression> , <assignmentStatement> )
||                        { <statement> }*
||                     END
|| *Note* Unless the interface to ParseAssignmentStatement() is changed, an ***UGLY*** PERIOD is 
||    required at end of each <assignmentStatement> above. To make the <FOR2Statement> syntax 
||    "prettier" requires a change to ParseAssignmentStatement() so that it doesn't "look for" the PERIOD
||    at the end of an <assignmentStatement>. So a temporary change has been made to 
||    ParseAssignmentStatement() by adding the formal parameter bool requirePERIOD = true and by
||    adding logic to optionally recognize the terminating PERIOD, but only when requirePERIOD is true.
||-----------------------------------------------------------
*/
   void ParseAssignmentStatement(TOKEN tokens[],const bool requirePERIOD=true);
   void ParseExpression(TOKEN tokens[],DATATYPE &datatype);
   void ParseStatement(TOKEN tokens[]);
   void GetNextToken(TOKEN tokens[]);

   char line[SOURCELINELENGTH+1];
   char E2label[SOURCELINELENGTH+1],S3label[SOURCELINELENGTH+1],
        Clabel[SOURCELINELENGTH+1],Elabel[SOURCELINELENGTH+1];
   char operand[SOURCELINELENGTH+1];
   DATATYPE datatype;

   EnterModule("FOR2Statement");

   sprintf(line,"; **** FOR2 statement (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   GetNextToken(tokens);

   if ( tokens[0].type != OPARENTHESIS )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '('");
   GetNextToken(tokens);

/*
    ...s1...                   ; (empty)
       PUSH      #0X0000       ; doS3 := FALSE
       JMP       S3????        ; doS3
E2???? EQU       *
    ...e2...                   ; e2
       SETT                    ; e2
       DISCARD   #0D1          ; (empty)
       JMPT      C????         ; (empty)
       JMP       E????         ; (empty)
S3???? EQU       *             ; doS3
       SETT                    ; doS3
       DISCARD   #0D1          ; (empty)
       JMPNT     E2????        ; (empty)
    ...s3...
       JMP       E2????        ; (empty)
C????  EQU       *             ; (empty)
    ...statements...
       PUSH      #0XFFFF       ; doS3 := TRUE
       JMP       S3????        ; doS3
E????  EQU       *             ; (empty)
*/
   ParseAssignmentStatement(tokens,false);
   if ( tokens[0].type != COMMA )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ','");
   GetNextToken(tokens);

// CODEGENERATION
   sprintf(E2label,"E2%04d",code.LabelSuffix());
   sprintf(S3label,"S3%04d",code.LabelSuffix());
   sprintf(Clabel, "C%04d", code.LabelSuffix());
   sprintf(Elabel, "E%04d", code.LabelSuffix());

   code.EmitFormattedLine("","PUSH","#0X0000","doS3 := FALSE");
   code.EmitFormattedLine("","JMP",S3label,"doS3");
   code.EmitFormattedLine(E2label,"EQU","*");
// ENDCODEGENERATION

   ParseExpression(tokens,datatype);
   if ( tokens[0].type != COMMA )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ','");
   GetNextToken(tokens);
   
// STATICSEMANTICS
   if ( datatype != BOOLTYPE )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting boolean expression");
// ENDSTATICSEMANTICS

// CODEGENERATION
   code.EmitFormattedLine("","SETT","","e2");
   code.EmitFormattedLine("","DISCARD","#0D1","(empty)");
   code.EmitFormattedLine("","JMPT",Clabel,"(empty)");
   code.EmitFormattedLine("","JMP",Elabel,"(empty)");
   code.EmitFormattedLine(S3label,"EQU","*","doS3");
   code.EmitFormattedLine("","SETT","","doS3");
   code.EmitFormattedLine("","DISCARD","#0D1","(empty)");
   code.EmitFormattedLine("","JMPNT",E2label,"(empty)");
// ENDCODEGENERATION

   ParseAssignmentStatement(tokens,false);

// CODEGENERATION
   code.EmitFormattedLine("","JMP",E2label,"(empty)");
   code.EmitFormattedLine(Clabel,"EQU","*","");
// ENDCODEGENERATION

   if ( tokens[0].type != CPARENTHESIS )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ')'");
   GetNextToken(tokens);

   while ( tokens[0].type != END )
      ParseStatement(tokens);

   GetNextToken(tokens);

// CODEGENERATION
   code.EmitFormattedLine("","PUSH","#0XFFFF","doS3 := TRUE");
   code.EmitFormattedLine("","JMP",S3label,"doS3");
   code.EmitFormattedLine(Elabel,"EQU","*");
// ENDCODEGENERATION

   ExitModule("FORStatement2");
}

//-----------------------------------------------------------
void ParseCHOOSEStatement(TOKEN tokens[])
//-----------------------------------------------------------
{
/*
||-----------------------------------------------------------
|| <CHOOSEStatement> ::= CHOOSE [ (( ONE | ALL )) ]
||                          { WHEN <expression> : { <statement> }* }*
||                          [ ELSE { <statement> }* ]
||                       END
||    *Notes* 
||       <expression>s are evaluated one-at-a-time from top-to-bottom
||       <expression>s must be boolean
||       mode ONE: (default) means execute <statement>s for only ONE <expression> WHEN true
||       mode ALL: execute <statement>s for ALL <expression>s WHEN true
||       ELSE means execute <statement>s when no <expression>s are true
||-----------------------------------------------------------
*/
   void ParseExpression(TOKEN tokens[],DATATYPE &datatype);
   void ParseStatement(TOKEN tokens[]);
   void GetNextToken(TOKEN tokens[]);

   char line[SOURCELINELENGTH+1];
   char Ilabel[SOURCELINELENGTH+1],Elabel1[SOURCELINELENGTH+1],Elabel2[SOURCELINELENGTH+1];
   DATATYPE datatype;
   char mode; // 'O' = ONE, 'A' = ALL

   EnterModule("CHOOSEStatement");

   sprintf(line,"; **** CHOOSE statement (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   GetNextToken(tokens);

   if       ( tokens[0].type == ONE )
   {
      mode = 'O';
      GetNextToken(tokens);
   } else if ( tokens[0].type == ALL )
   {
      mode = 'A';
      GetNextToken(tokens);
   } else
   {
      mode = 'O';
   }
/*
   Plan for the generalized CHOOSE statement with n WHENs, 1 ELSE (*Note* n
      can be 0 and the ELSE may be missing and the plan still "works.")

      PUSH      #0D0          ; count-of-true <expressions> := 0
   ...expression...           ; WHEN clause #1
      SETT
      DISCARD   #0D1
      JMPNT     I???2
      PUSH      #0D1          ; increment count-of-true <expressions>
      ADDI
   ...statements...
      JMP       E???1 when mode = ONE or "fall-through" when mode = ALL
I???2 EQU       *
   ...expression...           ; WHEN clause #2
      SETT
      DISCARD   #0D1
      JMPNT     I???3
      PUSH      #0D1          ; increment count-of-true <expressions>
      ADDI
   ...statements...
      JMP       E???1 when mode = ONE or "fall-through" when mode = ALL
      .
      .
I???n EQU       *
   ...expression...           ; WHEN clause #n
      SETT
      DISCARD   #0D1
      JMPNT     I???e
      PUSH      #0D1          ; increment count-of-true <expressions>
      ADDI
   ...statements...
      JMP       E???1 when mode = ONE or "fall-through" when mode = ALL
I???e EQU       *
E???1 EQU       *
      SETNZPI                 ; if ( count-of-true <expressions> = 0 ) goto E???2
      DISCARD   #0D1
      JMPNZ     E???2
   ...statements...           ; ELSE <statements> (if any)
E???2 EQU       *
*/

// CODEGENERATION
   sprintf(Elabel1,"E%04d",code.LabelSuffix());
   sprintf(Elabel2,"E%04d",code.LabelSuffix());
   code.EmitFormattedLine("","PUSH","#0D0","count-of-true <expressions> := 0");
// ENDCODEGENERATION

   while ( tokens[0].type == WHEN )
   {
      GetNextToken(tokens);
      ParseExpression(tokens,datatype);
      if ( datatype != BOOLTYPE )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting boolean expression");
      if ( tokens[0].type != COLON )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ':'");
      GetNextToken(tokens);

// CODEGENERATION
      code.EmitFormattedLine("","SETT");
      code.EmitFormattedLine("","DISCARD","#0D1");
      sprintf(Ilabel,"I%04d",code.LabelSuffix());
      code.EmitFormattedLine("","JMPNT",Ilabel);
      code.EmitFormattedLine("","PUSH","#0D1","increment count-of-true <expressions>");
      code.EmitFormattedLine("","ADDI");

// ENDCODEGENERATION

      while ( (tokens[0].type != WHEN) && 
              (tokens[0].type != ELSE) &&
              (tokens[0].type != END) )
          ParseStatement(tokens);

// CODEGENERATION
      if ( mode == 'O' )
         code.EmitFormattedLine("","JMP",Elabel1);
      code.EmitFormattedLine(Ilabel,"EQU","*");
// ENDCODEGENERATION
   }

// CODEGENERATION
   code.EmitFormattedLine(Elabel1,"EQU","*");
   code.EmitFormattedLine("","SETNZPI","","if ( count-of-true <expressions> = 0 ) goto E???2");
   code.EmitFormattedLine("","DISCARD","#0D1");
   code.EmitFormattedLine("","JMPNZ",Elabel2);
// ENDCODEGENERATION

   if ( tokens[0].type == ELSE )
   {
      GetNextToken(tokens);

      while ( tokens[0].type != END )
         ParseStatement(tokens);
   }

   if ( tokens[0].type != END )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting END");
   GetNextToken(tokens);

// CODEGENERATION
      code.EmitFormattedLine(Elabel2,"EQU","*");
// ENDCODEGENERATION

   ExitModule("CHOOSEStatement");
}

//-----------------------------------------------------------
void ParseDOTIMESStatement(TOKEN tokens[])
//-----------------------------------------------------------
{
/*
||-----------------------------------------------------------
|| <DOTIMESStatement> ::= DO2 <expression> TIMES
||                           { <statement> }*
||                        END
|| *Note* When <expression> <= 0, current semantics require that the
||    the body-of-loop <statement>s never execute. Should 
||    <expression> <= 0 be a run-time exception?! The <expression> is
||    evaluated only one time and its value does not change even when
||    the body-of-loop <statement>s make changes to variables occurring
||    in <expression>.
||-----------------------------------------------------------
*/
   void ParseExpression(TOKEN tokens[],DATATYPE &datatype);
   void ParseStatement(TOKEN tokens[]);
   void GetNextToken(TOKEN tokens[]);

   char line[SOURCELINELENGTH+1];
   char Dlabel[SOURCELINELENGTH+1],Elabel[SOURCELINELENGTH+1];
   char operand[SOURCELINELENGTH+1];
   DATATYPE datatype;

   EnterModule("DOTIMESStatement");

   sprintf(line,"; **** DOTIMES statement (%4d)",tokens[0].sourceLineNumber);
   code.EmitUnformattedLine(line);

   GetNextToken(tokens);

/*
      PUSH      #0D0          ; times := 0
   ...expression...
D???? EQU       *             ; times,e
      PUSH      SP:0D1        ; times,e,times
      PUSH      SP:0D1        ; times,e,times,e
      CMPI                    ; times,e (set LEG)
      JMPGE     E????         ; times,e
   ...statements...
      SWAP                    ; e,times
      PUSH      #0D1          ; e,times,1
      ADDI                    ; e,times := times+1
      SWAP                    ; times,e
      JMP       D????         ; times,e
E???? EQU       *             ; times,e
      DISCARD   #0D2          ; (empty)  
*/
// CODEGENERATION
   sprintf(Dlabel,"D%04d",code.LabelSuffix());
   sprintf(Elabel,"E%04d",code.LabelSuffix());
   code.EmitFormattedLine("","PUSH","#0D0","times := 0");
// ENDCODEGENERATION

   ParseExpression(tokens,datatype);

// STATICSEMANTICS
   if ( datatype != INTTYPE )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting integer expression");
// ENDSTATICSEMANTICS

// CODEGENERATION
   code.EmitFormattedLine(Dlabel,"EQU","*","times,e");
   code.EmitFormattedLine("","PUSH","SP:0D1","times,e,times");
   code.EmitFormattedLine("","PUSH","SP:0D1","times,e,times,e");
   code.EmitFormattedLine("","CMPI","","times,e (set LEG)");
   code.EmitFormattedLine("","JMPGE",Elabel,"times,e");
// ENDCODEGENERATION

   if ( tokens[0].type != TIMES )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting 'TIMES'");
   GetNextToken(tokens);

   while ( tokens[0].type != END )
      ParseStatement(tokens);
   GetNextToken(tokens);

// CODEGENERATION
   code.EmitFormattedLine("","SWAP","","e,times");
   code.EmitFormattedLine("","PUSH","#0D1","e,times,1");
   code.EmitFormattedLine("","ADDI","","e,times := times+1");
   code.EmitFormattedLine("","SWAP","","times,e");
   code.EmitFormattedLine("","JMP",Dlabel,"times,e");
   code.EmitFormattedLine(Elabel,"EQU","*");
   code.EmitFormattedLine("","DISCARD","#0D2","(empty)");
// ENDCODEGENERATION

   ExitModule("DOTIMESStatement");
}
//-----------------------------------------------------------
// End added ***extras*** of SPL5 language
//-----------------------------------------------------------

//-----------------------------------------------------------
//-----------------------------------------------------------
void ParseExpression(TOKEN tokens[], DATATYPE &datatype)
//-----------------------------------------------------------
{
// CODEGENERATION
/*
   An expression is composed of a collection of one or more operands (SPL calls them
      primaries) and operators (and perhaps sets of parentheses to modify the default 
      order-of-evaluation established by precedence and associativity rules).
      Expression evaluation computes a single value as the expression's result.
      The result has a specific data type. By design, the expression result is 
      "left" at the top of the run-time stack for subsequent use.
   
   SPL expressions must be single-mode with operators working on operands of
      the appropriate type (for example, boolean AND boolean) and not mixing
      modes. Static semantic analysis guarantees that operators are
      operating on operands of appropriate data type.
*/
// ENDCODEGENERATION

   void ParseConjunction(TOKEN tokens[], DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   DATATYPE datatypeLHS, datatypeRHS;

   EnterModule("Expression");

   ParseConjunction(tokens, datatypeLHS);

   if ((tokens[0].type == OR) ||
       (tokens[0].type == LOGICAL_OR) ||
       (tokens[0].type == NOR) ||
       (tokens[0].type == XOR))
   {
      while ((tokens[0].type == OR) ||
             (tokens[0].type == LOGICAL_OR) ||
             (tokens[0].type == NOR) ||
             (tokens[0].type == XOR))
      {
         TOKENTYPE operation = tokens[0].type;

         GetNextToken(tokens);
         ParseConjunction(tokens, datatypeRHS);
   
// CODEGENERATION
         switch (operation)
         {
            case OR:
               // STATICSEMANTICS
               if (!(datatypeLHS == BOOLTYPE && datatypeRHS == BOOLTYPE))
                  ProcessCompilerError(tokens[0].sourceLineNumber, tokens[0].sourceLineIndex, "Expecting boolean operands");
               // ENDSTATICSEMANTICS
   
               code.EmitFormattedLine("", "OR");
               datatype = BOOLTYPE;
               break;

            case LOGICAL_OR:
               // STATICSEMANTICS
               if (!(datatypeLHS == BOOLTYPE && datatypeRHS == BOOLTYPE))
                  ProcessCompilerError(tokens[0].sourceLineNumber, tokens[0].sourceLineIndex, "Expecting boolean operands");
               // ENDSTATICSEMANTICS
   
               code.EmitFormattedLine("", "||");  // Emit || for LOGICAL_OR
               datatype = BOOLTYPE;
               break;

            case NOR:
               // STATICSEMANTICS
               if (!(datatypeLHS == BOOLTYPE && datatypeRHS == BOOLTYPE))
                  ProcessCompilerError(tokens[0].sourceLineNumber, tokens[0].sourceLineIndex, "Expecting boolean operands");
               // ENDSTATICSEMANTICS
   
               code.EmitFormattedLine("", "NOR");
               datatype = BOOLTYPE;
               break;

            case XOR:
               // STATICSEMANTICS
               if (!(datatypeLHS == BOOLTYPE && datatypeRHS == BOOLTYPE))
                  ProcessCompilerError(tokens[0].sourceLineNumber, tokens[0].sourceLineIndex, "Expecting boolean operands");
               // ENDSTATICSEMANTICS
   
               code.EmitFormattedLine("", "XOR");
               datatype = BOOLTYPE;
               break;
         }
      }
// CODEGENERATION

   }
   else
      datatype = datatypeLHS;

   ExitModule("Expression");
}


//-----------------------------------------------------------
void ParseConjunction(TOKEN tokens[], DATATYPE &datatype)
//-----------------------------------------------------------
{
   void ParseNegation(TOKEN tokens[], DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   DATATYPE datatypeLHS, datatypeRHS;

   EnterModule("Conjunction");

   ParseNegation(tokens, datatypeLHS);

   // Check for both AND and LOGICAL_AND
   if ((tokens[0].type == AND) || (tokens[0].type == LOGICAL_AND) || (tokens[0].type == NAND))
   {
      while ((tokens[0].type == AND) || (tokens[0].type == LOGICAL_AND) || (tokens[0].type == NAND))
      {
         TOKENTYPE operation = tokens[0].type;

         GetNextToken(tokens);
         ParseNegation(tokens, datatypeRHS);

         switch (operation)
         {
            case AND:
               if (!(datatypeLHS == BOOLTYPE && datatypeRHS == BOOLTYPE))
                  ProcessCompilerError(tokens[0].sourceLineNumber, tokens[0].sourceLineIndex, "Expecting boolean operands");
               code.EmitFormattedLine("", "AND");  // Emit AND for original AND operator
               datatype = BOOLTYPE;
               break;

            case LOGICAL_AND:
               if (!(datatypeLHS == BOOLTYPE && datatypeRHS == BOOLTYPE))
                  ProcessCompilerError(tokens[0].sourceLineNumber, tokens[0].sourceLineIndex, "Expecting boolean operands");
               code.EmitFormattedLine("", "&&");  // Emit && for LOGICAL_AND
               datatype = BOOLTYPE;
               break;

            case NAND:
               if (!(datatypeLHS == BOOLTYPE && datatypeRHS == BOOLTYPE))
                  ProcessCompilerError(tokens[0].sourceLineNumber, tokens[0].sourceLineIndex, "Expecting boolean operands");
               code.EmitFormattedLine("", "NAND");
               datatype = BOOLTYPE;
               break;
         }
      }
   }
   else
      datatype = datatypeLHS;

   ExitModule("Conjunction");
}


//-----------------------------------------------------------
void ParseNegation(TOKEN tokens[],DATATYPE &datatype)
//-----------------------------------------------------------
{
   void ParseComparison(TOKEN tokens[],DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   DATATYPE datatypeRHS;

   EnterModule("Negation");

   if ( tokens[0].type == NOT )
   {
      GetNextToken(tokens);
      ParseComparison(tokens,datatypeRHS);

      if ( !(datatypeRHS == BOOLTYPE) )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting boolean operand");
      code.EmitFormattedLine("","NOT");
      datatype = BOOLTYPE;
   }
   else
      ParseComparison(tokens,datatype);

   ExitModule("Negation");
}

//-----------------------------------------------------------
void ParseComparison(TOKEN tokens[],DATATYPE &datatype)
//-----------------------------------------------------------
{
   void ParseComparator(TOKEN tokens[],DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   DATATYPE datatypeLHS,datatypeRHS;

   EnterModule("Comparison");

   ParseComparator(tokens,datatypeLHS);
   if ( (tokens[0].type ==    LT) ||
        (tokens[0].type ==  LTEQ) ||
        (tokens[0].type ==    EQ) ||
        (tokens[0].type ==    GT) ||
        (tokens[0].type ==  GTEQ) ||
        (tokens[0].type == NOTEQ)
      )
   {
      TOKENTYPE operation = tokens[0].type;

      GetNextToken(tokens);
      ParseComparator(tokens,datatypeRHS);

      if ( (datatypeLHS != INTTYPE) || (datatypeRHS != INTTYPE) )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting integer operands");
/*
      CMPI
      JMPXX     T????         ; XX = L,E,G,LE,NE,GE (as required)
      PUSH      #0X0000       ; push FALSE
      JMP       E????         ;    or 
T???? PUSH      #0XFFFF       ; push TRUE (as required)
E???? EQU       *
*/
      char Tlabel[SOURCELINELENGTH+1],Elabel[SOURCELINELENGTH+1];

      code.EmitFormattedLine("","CMPI");
      sprintf(Tlabel,"T%04d",code.LabelSuffix());
      sprintf(Elabel,"E%04d",code.LabelSuffix());
      switch ( operation )
      {
         case LT:
            code.EmitFormattedLine("","JMPL",Tlabel);
            break;
         case LTEQ:
            code.EmitFormattedLine("","JMPLE",Tlabel);
            break;
         case EQ:
            code.EmitFormattedLine("","JMPE",Tlabel);
            break;
         case GT:
            code.EmitFormattedLine("","JMPG",Tlabel);
            break;
         case GTEQ:
            code.EmitFormattedLine("","JMPGE",Tlabel);
            break;
         case NOTEQ:
            code.EmitFormattedLine("","JMPNE",Tlabel);
            break;
      }
      datatype = BOOLTYPE;
      code.EmitFormattedLine("","PUSH","#0X0000");
      code.EmitFormattedLine("","JMP",Elabel);
      code.EmitFormattedLine(Tlabel,"PUSH","#0XFFFF");
      code.EmitFormattedLine(Elabel,"EQU","*");
   }
   else
      datatype = datatypeLHS;

   ExitModule("Comparison");
}

//-----------------------------------------------------------
void ParseComparator(TOKEN tokens[],DATATYPE &datatype)
//-----------------------------------------------------------
{
   void ParseTerm(TOKEN tokens[],DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   DATATYPE datatypeLHS,datatypeRHS;

   EnterModule("Comparator");

   ParseTerm(tokens,datatypeLHS);

   if ( (tokens[0].type ==  PLUS) ||
        (tokens[0].type == MINUS) )
   {
      while ( (tokens[0].type ==  PLUS) ||
              (tokens[0].type == MINUS) )
      {
         TOKENTYPE operation = tokens[0].type;
         
         GetNextToken(tokens);
         ParseTerm(tokens,datatypeRHS);

         if ( (datatypeLHS != INTTYPE) || (datatypeRHS != INTTYPE) )
            ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting integer operands");

         switch ( operation )
         {
            case PLUS:
               code.EmitFormattedLine("","ADDI");
               break;
            case MINUS:
               code.EmitFormattedLine("","SUBI");
               break;
         }
         datatype = INTTYPE;
      }
   }
   else
      datatype = datatypeLHS;
   
   ExitModule("Comparator");
}

//-----------------------------------------------------------
void ParseTerm(TOKEN tokens[],DATATYPE &datatype)
//-----------------------------------------------------------
{
   void ParseFactor(TOKEN tokens[],DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   DATATYPE datatypeLHS,datatypeRHS;

   EnterModule("Term");

   ParseFactor(tokens,datatypeLHS);
   if ( (tokens[0].type == MULTIPLY) ||
        (tokens[0].type ==   DIVIDE) ||
        (tokens[0].type ==  MODULUS) )
   {
      while ( (tokens[0].type == MULTIPLY) ||
              (tokens[0].type ==   DIVIDE) ||
              (tokens[0].type ==  MODULUS) )
      {
         TOKENTYPE operation = tokens[0].type;
         
         GetNextToken(tokens);
         ParseFactor(tokens,datatypeRHS);

         if ( (datatypeLHS != INTTYPE) || (datatypeRHS != INTTYPE) )
            ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting integer operands");

         switch ( operation )
         {
            case MULTIPLY:
               code.EmitFormattedLine("","MULI");
               break;
            case DIVIDE:
               code.EmitFormattedLine("","DIVI");
               break;
            case MODULUS:
               code.EmitFormattedLine("","REMI");
               break;
         }
         datatype = INTTYPE;
      }
   }
   else
      datatype = datatypeLHS;

   ExitModule("Term");
}

//-----------------------------------------------------------
void ParseFactor(TOKEN tokens[],DATATYPE &datatype)
//-----------------------------------------------------------
{
   void ParseSecondary(TOKEN tokens[],DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   EnterModule("Factor");

   if ( (tokens[0].type ==   ABS) ||
        (tokens[0].type ==  PLUS) ||
        (tokens[0].type == MINUS)
      )
   {
      DATATYPE datatypeRHS;
      TOKENTYPE operation = tokens[0].type;

      GetNextToken(tokens);
      ParseSecondary(tokens,datatypeRHS);

      if ( datatypeRHS != INTTYPE )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting integer operand");

      switch ( operation )
      {
         case ABS:
/*
      SETNZPI
      JMPNN     E????
      NEGI                    ; NEGI or NEGF (as required)
E???? EQU       *
*/
            {
               char Elabel[SOURCELINELENGTH+1];
         
               sprintf(Elabel,"E%04d",code.LabelSuffix());
               code.EmitFormattedLine("","SETNZPI");
               code.EmitFormattedLine("","JMPNN",Elabel);
               code.EmitFormattedLine("","NEGI");
               code.EmitFormattedLine(Elabel,"EQU","*");
            }
            break;
         case PLUS:
         // Do nothing (identity operator)
            break;
         case MINUS:
            code.EmitFormattedLine("","NEGI");
            break;
      }
      datatype = INTTYPE;
   }
   else
      ParseSecondary(tokens,datatype);

   ExitModule("Factor");
}

//-----------------------------------------------------------
void ParseSecondary(TOKEN tokens[],DATATYPE &datatype)
//-----------------------------------------------------------
{
   void ParsePrefix(TOKEN tokens[],DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   DATATYPE datatypeLHS,datatypeRHS;

   EnterModule("Secondary");

   ParsePrefix(tokens,datatypeLHS);

   if ( tokens[0].type == POWER )
   {
      GetNextToken(tokens);

      ParsePrefix(tokens,datatypeRHS);

      if ( (datatypeLHS != INTTYPE) || (datatypeRHS != INTTYPE) )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting integer operands");

      code.EmitFormattedLine("","POWI");
      datatype = INTTYPE;
   }
   else
      datatype = datatypeLHS;

   ExitModule("Secondary");
}

//-----------------------------------------------------------
void ParsePrefix(TOKEN tokens[],DATATYPE &datatype)
//-----------------------------------------------------------
{
   void ParseVariable(TOKEN tokens[],bool asLValue,DATATYPE &datatype);
   void ParsePrimary(TOKEN tokens[],DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   EnterModule("Prefix");

   if ( (tokens[0].type == INC) ||
        (tokens[0].type == DEC)
      )
   {
      DATATYPE datatypeRHS;
      TOKENTYPE operation = tokens[0].type;

      GetNextToken(tokens);
      ParseVariable(tokens,true,datatypeRHS);

      if ( datatypeRHS != INTTYPE )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting integer operand");

      switch ( operation )
      {
         case INC:
            code.EmitFormattedLine("","PUSH","@SP:0D0");
            code.EmitFormattedLine("","PUSH","#0D1");
            code.EmitFormattedLine("","ADDI");
            code.EmitFormattedLine("","POP","@SP:0D1");       // side-effect
            code.EmitFormattedLine("","PUSH","@SP:0D0");
            code.EmitFormattedLine("","SWAP");
            code.EmitFormattedLine("","DISCARD","#0D1");      // value
            break;
         case DEC:
            code.EmitFormattedLine("","PUSH","@SP:0D0");
            code.EmitFormattedLine("","PUSH","#0D1");
            code.EmitFormattedLine("","SUBI");
            code.EmitFormattedLine("","POP","@SP:0D1");       // side-effect
            code.EmitFormattedLine("","PUSH","@SP:0D0");
            code.EmitFormattedLine("","SWAP");
            code.EmitFormattedLine("","DISCARD","#0D1");      // value
            break;
      }
      datatype = INTTYPE;
   }
   else
      ParsePrimary(tokens,datatype);

   ExitModule("Prefix");
}

//-----------------------------------------------------------
void ParsePrimary(TOKEN tokens[],DATATYPE &datatype)
//-----------------------------------------------------------
{
   void ParseVariable(TOKEN tokens[],bool asLValue,DATATYPE &datatype);
   void ParseExpression(TOKEN tokens[],DATATYPE &datatype);
   void GetNextToken(TOKEN tokens[]);

   EnterModule("Primary");

   switch ( tokens[0].type )
   {
      case INTEGER:
         {
            char operand[SOURCELINELENGTH+1];
            
            sprintf(operand,"#0D%s",tokens[0].lexeme);
            code.EmitFormattedLine("","PUSH",operand);
            datatype = INTTYPE;
            GetNextToken(tokens);
         }
         break;
   //************* Thanks to Cayden Garcia (FA2023)
   // ***BEWARE*** when you choose a different lexeme for either boolean value!
   //*************
      case TRUE:
         code.EmitFormattedLine("","PUSH","#0XFFFF"); // or "#true"
         datatype = BOOLTYPE;
         GetNextToken(tokens);
         break;
      case FALSE:
         code.EmitFormattedLine("","PUSH","#0X0000"); // or "false"
         datatype = BOOLTYPE;
         GetNextToken(tokens);
         break;
      case OPARENTHESIS:
         GetNextToken(tokens);
         ParseExpression(tokens,datatype);
         if ( tokens[0].type != CPARENTHESIS )
            ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ')'");
         GetNextToken(tokens);
         break;
      case IDENTIFIER:
         {
            bool isInTable;
            int index;
   
            index = identifierTable.GetIndex(tokens[0].lexeme,isInTable);
            if ( !isInTable )
               ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Undefined identifier");
//==========================
// variable reference
//==========================
            if ( identifierTable.GetType(index) != FUNCTION_SUBPROGRAMMODULE )
            {
            // array variable dimension attribute operation
               if ( (identifierTable.GetDimensions(index) > 0) && 
                    ((tokens[1].type == LB) || (tokens[1].type == UB)) )
               {
                  TOKENTYPE dimensionOperator;
                  DATATYPE dimensionDatatype;

                  GetNextToken(tokens);
                  dimensionOperator = tokens[0].type;
                  GetNextToken(tokens);
                  if ( tokens[0].type != OPARENTHESIS )
                     ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '('");
                  GetNextToken(tokens);
                  ParseExpression(tokens,dimensionDatatype);
                  if ( dimensionDatatype != INTTYPE )
                     ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Dimension expression must be integer");
                  datatype = INTTYPE;
                  if ( tokens[0].type != CPARENTHESIS )
                     ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ')'");
                  GetNextToken(tokens);

// CODEGENERATION
                  if    ( dimensionOperator == LB )
                     code.EmitFormattedLine("","GETALB",identifierTable.GetReference(index));
                  else//( dimensionOperator == UB )
                     code.EmitFormattedLine("","GETAUB",identifierTable.GetReference(index));
// ENDCODEGENERATION

               }
            // scalar variable or array variable element reference 
               else
                  ParseVariable(tokens,false,datatype);
            }
//==========================
// FUNCTION_SUBPROGRAMMODULE reference
//==========================
            else
            {
               char operand[MAXIMUMLENGTHIDENTIFIER+1];
               int parameters;

               GetNextToken(tokens);
               if ( tokens[0].type != OPARENTHESIS )
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '('");

// CODEGENERATION
               code.EmitFormattedLine("","PUSH","#0X0000","reserve space for function return value");
// ENDCODEGENERATION

               datatype = identifierTable.GetDatatype(index);
               parameters = 0;
               if ( tokens[1].type == CPARENTHESIS )
               {
                  GetNextToken(tokens);
               }
               else
               {
                  do
                  {
                     DATATYPE expressionDatatype;

                     GetNextToken(tokens);
                     ParseExpression(tokens,expressionDatatype);
                     parameters++;
                     
// STATICSEMANTICS
                     if ( expressionDatatype != identifierTable.GetDatatype(index+parameters) )
                        ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
                           "Actual parameter data type does not match formal parameter data type");
// ENDSTATICSEMANTICS

                  } while ( tokens[0].type == COMMA );
               }
                     
// STATICSEMANTICS
               if ( identifierTable.GetCountOfFormalParameters(index) != parameters )
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
                     "Number of actual parameters does not match number of formal parameters");
// ENDSTATICSEMANTICS

               if ( tokens[0].type != CPARENTHESIS )
                  ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ')'");
               GetNextToken(tokens);

// CODEGENERATION
               code.EmitFormattedLine("","PUSHFB");
               code.EmitFormattedLine("","CALL",identifierTable.GetReference(index));
               code.EmitFormattedLine("","POPFB");
               sprintf(operand,"#0D%d",parameters);
               code.EmitFormattedLine("","DISCARD",operand);
// ENDCODEGENERATION
            }
         }
         break;
      default:
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
                              "Expecting integer, true, false, '(', variable, FUNCTION identifier");
         break;
   }

   ExitModule("Primary");
}

//-----------------------------------------------------------
void ParseVariable(TOKEN tokens[],bool asLValue,DATATYPE &datatype)
//-----------------------------------------------------------
{
/*
Syntax "locations"                 l- or r-value
---------------------------------  -------------
<expression>                       r-value
<prefix>                           l-value
<INPUTStatement>                   l-value
LHS of <assignmentStatement>       l-value
<FORStatement>                     l-value
OUT <formalParameter>              l-value
IO <formalParameter>               l-value
REF <formalParameter>              l-value

r-value ( read-only): value is pushed on run-time stack
l-value (read/write): address of value is pushed on run-time stack
*/
   void GetNextToken(TOKEN tokens[]);

   bool isInTable;
   int index;
   int dimensions;
   IDENTIFIERTYPE identifierType;

   EnterModule("Variable");

   if ( tokens[0].type != IDENTIFIER )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting identifier");

// STATICSEMANTICS
   index = identifierTable.GetIndex(tokens[0].lexeme,isInTable);
   if ( !isInTable )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Undefined identifier");
   
   identifierType = identifierTable.GetType(index);
   datatype = identifierTable.GetDatatype(index);

   if ( !((identifierType ==           GLOBAL_VARIABLE) ||
          (identifierType ==           GLOBAL_CONSTANT) ||
          (identifierType ==    PROGRAMMODULE_VARIABLE) ||
          (identifierType ==    PROGRAMMODULE_CONSTANT) || 
          (identifierType == SUBPROGRAMMODULE_VARIABLE) ||
          (identifierType == SUBPROGRAMMODULE_CONSTANT) ||
          (identifierType ==              IN_PARAMETER) ||
          (identifierType ==             OUT_PARAMETER) ||
          (identifierType ==              IO_PARAMETER) ||
          (identifierType ==             REF_PARAMETER)) ) 
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting variable or constant identifier");
      
   if ( asLValue && ((identifierType ==           GLOBAL_CONSTANT) || 
                     (identifierType ==    PROGRAMMODULE_CONSTANT) ||
                     (identifierType == SUBPROGRAMMODULE_CONSTANT)) )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Constant may not be l-value");

   if ( asLValue && (identifierType == GLOBAL_VARIABLE) && code.IsInModuleBody(FUNCTION_SUBPROGRAMMODULE) )
      ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"FUNCTION may not modify global variable");
// ENDSTATICSEMANTICS

// CODEGENERATION
   if ( identifierTable.GetDimensions(index) == 0 )
   {
      if ( asLValue )
         code.EmitFormattedLine("","PUSHA",identifierTable.GetReference(index));
      else
         code.EmitFormattedLine("","PUSH",identifierTable.GetReference(index));
   }
   else
   {
      GetNextToken(tokens);
      if ( tokens[0].type != OBRACKET )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting '['");
      dimensions = 0;
      do
      {
         DATATYPE expressionDatatype;

         GetNextToken(tokens);
         ParseExpression(tokens,expressionDatatype);
         dimensions++;
         if ( expressionDatatype != INTTYPE )
            ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Index expression must be integer");
      } while ( tokens[0].type == COMMA );
         
      if ( identifierTable.GetDimensions(index) != dimensions )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,
            "Number of index expressions does not match array dimensions");

      if ( tokens[0].type != CBRACKET )
         ProcessCompilerError(tokens[0].sourceLineNumber,tokens[0].sourceLineIndex,"Expecting ']'");

      if ( asLValue )
         code.EmitFormattedLine("","ADRAE",identifierTable.GetReference(index));
      else
         code.EmitFormattedLine("","GETAE",identifierTable.GetReference(index));
   }
// ENDCODEGENERATION

   GetNextToken(tokens);

   ExitModule("Variable");
}

//-----------------------------------------------------------
void Callback1(int sourceLineNumber,const char sourceLine[])
//-----------------------------------------------------------
{
   cout << setw(4) << sourceLineNumber << " " << sourceLine << endl;
}

//-----------------------------------------------------------
void Callback2(int sourceLineNumber,const char sourceLine[])
//-----------------------------------------------------------
{
   char line[SOURCELINELENGTH+1];

// CODEGENERATION
   sprintf(line,"; %4d %s",sourceLineNumber,sourceLine);
   code.EmitUnformattedLine(line);
// ENDCODEGENERATION
}

//-----------------------------------------------------------
void GetNextToken(TOKEN tokens[])
//-----------------------------------------------------------
{
   const char *TokenDescription(TOKENTYPE type);

   int i;
   TOKENTYPE type;
   char lexeme[SOURCELINELENGTH+1];
   int sourceLineNumber;
   int sourceLineIndex;
   char information[SOURCELINELENGTH+1];

//============================================================
// Move look-ahead "window" to make room for next token/lexeme
//============================================================
   for (int i = 1; i <= LOOKAHEAD; i++)
      tokens[i-1] = tokens[i];

   char nextCharacter = reader.GetLookAheadCharacter(0).character;

//============================================================
// "Eat" white space and comments
//============================================================
   do
   {
//    "Eat" any white-space (blanks and EOLCs and TABCs) 
      while ( (nextCharacter == ' ')
           || (nextCharacter == READER<CALLBACKSUSED>::EOLC)
           || (nextCharacter == READER<CALLBACKSUSED>::TABC) )
         nextCharacter = reader.GetNextCharacter().character;

//    "Eat" line comment
      if ( nextCharacter == ';' )
      {

#ifdef TRACESCANNER
   sprintf(information,"At (%4d:%3d) begin line comment",
      reader.GetLookAheadCharacter(0).sourceLineNumber,
      reader.GetLookAheadCharacter(0).sourceLineIndex);
   lister.ListInformationLine(information);
#endif

         do
            nextCharacter = reader.GetNextCharacter().character;
         while ( nextCharacter != READER<CALLBACKSUSED>::EOLC );
      }
      if (nextCharacter == '&' && reader.GetLookAheadCharacter(1).character == '&')
      {
        type = LOGICAL_AND;
        lexeme[0] = '&'; lexeme[1] = '&'; lexeme[2] = '\0';
        reader.GetNextCharacter();
        reader.GetNextCharacter();
      }
      else if (nextCharacter == '|' && reader.GetLookAheadCharacter(1).character == '|')
      {
        type = LOGICAL_OR;
        lexeme[0] = '|'; lexeme[1] = '|'; lexeme[2] = '\0';
        reader.GetNextCharacter();
        reader.GetNextCharacter();
      }
//    "Eat" block comments (nesting allowed)
      if ( (nextCharacter == '/') && (reader.GetLookAheadCharacter(1).character == '*') )
      {
         int depth = 0;

         do
         {
            if      ( (nextCharacter == '/') && (reader.GetLookAheadCharacter(1).character == '*') )
            {
               depth++;

#ifdef TRACESCANNER
   sprintf(information,"At (%4d:%3d) begin block comment depth = %d",
      reader.GetLookAheadCharacter(0).sourceLineNumber,
      reader.GetLookAheadCharacter(0).sourceLineIndex,
      depth);
   lister.ListInformationLine(information);
#endif

               nextCharacter = reader.GetNextCharacter().character;
               nextCharacter = reader.GetNextCharacter().character;
            }
            else if ( (nextCharacter == '*') && (reader.GetLookAheadCharacter(1).character == '/') )
            {

#ifdef TRACESCANNER
   sprintf(information,"At (%4d:%3d)   end block comment depth = %d",
      reader.GetLookAheadCharacter(0).sourceLineNumber,
      reader.GetLookAheadCharacter(0).sourceLineIndex,
      depth);
   lister.ListInformationLine(information);
#endif

               depth--;
               nextCharacter = reader.GetNextCharacter().character;
               nextCharacter = reader.GetNextCharacter().character;
            }
            else
               nextCharacter = reader.GetNextCharacter().character;
         }
         while ( (depth != 0) && (nextCharacter != READER<CALLBACKSUSED>::EOPC) );
         if ( depth != 0 ) 
            ProcessCompilerError(reader.GetLookAheadCharacter(0).sourceLineNumber,
                                 reader.GetLookAheadCharacter(0).sourceLineIndex,
                                 "Unexpected end-of-program");
      }
   } while ( (nextCharacter == ' ')
          || (nextCharacter == READER<CALLBACKSUSED>::EOLC)
          || (nextCharacter == READER<CALLBACKSUSED>::TABC)
          || (nextCharacter == ';')
          || ((nextCharacter == '/') && (reader.GetLookAheadCharacter(1).character == '*')) );

//============================================================
// Scan token
//============================================================
   sourceLineNumber = reader.GetLookAheadCharacter(0).sourceLineNumber;
   sourceLineIndex = reader.GetLookAheadCharacter(0).sourceLineIndex;

// reserved word or <identifier>
   if      ( isalpha(nextCharacter) )
   {
      char UCLexeme[SOURCELINELENGTH+1];

      i = 0;
      lexeme[i++] = nextCharacter;
      nextCharacter = reader.GetNextCharacter().character;
      while ( isalpha(nextCharacter) || isdigit(nextCharacter) || (nextCharacter == '_') )
      {
         lexeme[i++] = nextCharacter;
         nextCharacter = reader.GetNextCharacter().character;
      }
      lexeme[i] = '\0';
      for (i = 0; i <= (int) strlen(lexeme); i++)
         UCLexeme[i] = toupper(lexeme[i]);

      bool isFound = false;

      i = 0;
      while ( !isFound && (i <= (sizeof(TOKENTABLE)/sizeof(TOKENTABLERECORD))-1) )
      {
         if ( TOKENTABLE[i].isReservedWord && (strcmp(UCLexeme,TOKENTABLE[i].description) == 0) )
            isFound = true;
         else
            i++;
      }
      if ( isFound )
         type = TOKENTABLE[i].type;
      else
         type = IDENTIFIER;
   }
// <integer>
   else if ( isdigit(nextCharacter) )
   {
      i = 0;
      lexeme[i++] = nextCharacter;
      nextCharacter = reader.GetNextCharacter().character;
      while ( isdigit(nextCharacter) )
      {
         lexeme[i++] = nextCharacter;
         nextCharacter = reader.GetNextCharacter().character;
      }
      lexeme[i] = '\0';
      type = INTEGER;
   }
   else
   {
      switch ( nextCharacter )
      {
// <string> literal *Note* escape character sequences \n,\t,\b,\r,\\,\" supported
         case '"': 
            i = 0;
            nextCharacter = reader.GetNextCharacter().character;
            while ( (nextCharacter != '"')
                 && (nextCharacter != READER<CALLBACKSUSED>::EOLC)
                 && (nextCharacter != READER<CALLBACKSUSED>::EOPC) )
            {
               if ( nextCharacter == '\\' )
               {
                  lexeme[i++] = nextCharacter;
                  nextCharacter = reader.GetNextCharacter().character;
                  if ( (nextCharacter ==  'n') ||
                       (nextCharacter ==  't') ||
                       (nextCharacter ==  'b') ||
                       (nextCharacter ==  'r') ||
                       (nextCharacter == '\\') ||
                       (nextCharacter ==  '"') )
                  {
                     lexeme[i++] = nextCharacter;
                  }
                  else
                     ProcessCompilerError(sourceLineNumber,sourceLineIndex,
                                          "Illegal escape character sequence in string literal");
               }
               else
               {
                  lexeme[i++] = nextCharacter;
               }
               nextCharacter = reader.GetNextCharacter().character;
            }
            if ( nextCharacter != '"' ) 
               ProcessCompilerError(sourceLineNumber,sourceLineIndex,
                                    "Un-terminated string literal");
            lexeme[i] = '\0';
            type = STRING;
            reader.GetNextCharacter();
            break;
         case READER<CALLBACKSUSED>::EOPC: 
            {
               static int count = 0;
   
               if ( ++count > (LOOKAHEAD+1) )
                  ProcessCompilerError(sourceLineNumber,sourceLineIndex,
                                       "Unexpected end-of-program");
               else
               {
                  type = EOPTOKEN;
                  reader.GetNextCharacter();
                  lexeme[0] = '\0';
               }
            }
            break;
         case ',':
            type = COMMA;
            lexeme[0] = nextCharacter; lexeme[1] = '\0';
            reader.GetNextCharacter();
            break;
         case '.': 
            type = PERIOD;
            lexeme[0] = nextCharacter; lexeme[1] = '\0';
            reader.GetNextCharacter();
            break;
         case '(': 
            type = OPARENTHESIS;
            lexeme[0] = nextCharacter; lexeme[1] = '\0';
            reader.GetNextCharacter();
            break;
         case ')': 
            type = CPARENTHESIS;
            lexeme[0] = nextCharacter; lexeme[1] = '\0';
            reader.GetNextCharacter();
            break;
         case ':': 
            lexeme[0] = nextCharacter;
            nextCharacter = reader.GetNextCharacter().character;
            if ( nextCharacter == '=' )
            {
               type = COLONEQ;
               lexeme[1] = nextCharacter; lexeme[2] = '\0';
               reader.GetNextCharacter();
            }
            else
            {
               type = COLON;
               lexeme[1] = '\0';
            }
            break;
         case '{': 
            type = OBRACE;
            lexeme[0] = nextCharacter; lexeme[1] = '\0';
            reader.GetNextCharacter();
            break;
         case '}': 
            type = CBRACE;
            lexeme[0] = nextCharacter; lexeme[1] = '\0';
            reader.GetNextCharacter();
            break;
         case '<': 
            lexeme[0] = nextCharacter;
            nextCharacter = reader.GetNextCharacter().character;
            if      ( nextCharacter == '=' )
            {
               type = LTEQ;
               lexeme[1] = nextCharacter; lexeme[2] = '\0';
               reader.GetNextCharacter();
            }
            else if ( nextCharacter == '>' )
            {
               type = NOTEQ;
               lexeme[1] = nextCharacter; lexeme[2] = '\0';
               reader.GetNextCharacter();
            }
            else
            {
               type = LT;
               lexeme[1] = '\0';
            }
            break;
         case '=': 
            type = EQ;
            lexeme[0] = nextCharacter; lexeme[1] = '\0';
            reader.GetNextCharacter();
            break;
         case '>':
            lexeme[0] = nextCharacter;
            nextCharacter = reader.GetNextCharacter().character;
            if ( nextCharacter == '=' )
            {
               type = GTEQ;
               lexeme[1] = nextCharacter; lexeme[2] = '\0';
               reader.GetNextCharacter();
            }
            else
            {
               type = GT;
               lexeme[1] = '\0';
            }
            break;
      // use character look-ahead to "find" '='
         case '!':
            lexeme[0] = nextCharacter;
            if ( reader.GetLookAheadCharacter(1).character == '=' )
            {
               nextCharacter = reader.GetNextCharacter().character;
               lexeme[1] = nextCharacter; lexeme[2] = '\0';
               reader.GetNextCharacter();
               type = NOTEQ;
            }
            else
            {
               type = UNKTOKEN;
               lexeme[1] = '\0';
               reader.GetNextCharacter();
            }
            break;
         case '+': 
            lexeme[0] = nextCharacter;
            if ( reader.GetLookAheadCharacter(1).character == '+' )
            {
               nextCharacter = reader.GetNextCharacter().character;
               lexeme[1] = nextCharacter; lexeme[2] = '\0';
               type = INC;
            }
            else
            {
               type = PLUS;
               lexeme[0] = nextCharacter; lexeme[1] = '\0';
            }
            reader.GetNextCharacter();
            break;
         case '-': 
            lexeme[0] = nextCharacter;
            if ( reader.GetLookAheadCharacter(1).character == '-' )
            {
               nextCharacter = reader.GetNextCharacter().character;
               lexeme[1] = nextCharacter; lexeme[2] = '\0';
               type = DEC;
            }
            else
            {
               type = MINUS;
               lexeme[0] = nextCharacter; lexeme[1] = '\0';
            }
            reader.GetNextCharacter();
            break;
      // use character look-ahead to "find" other '*'
         case '*': 
            lexeme[0] = nextCharacter;
            if ( reader.GetLookAheadCharacter(1).character == '*' )
            {
               nextCharacter = reader.GetNextCharacter().character;
               lexeme[1] = nextCharacter; lexeme[2] = '\0';
               type = POWER;
            }
            else
            {
               type = MULTIPLY;
               lexeme[0] = nextCharacter; lexeme[1] = '\0';
            }
            reader.GetNextCharacter();
            break;
         case '/': 
            type = DIVIDE;
            lexeme[0] = nextCharacter; lexeme[1] = '\0';
            reader.GetNextCharacter();
            break;
         case '%': 
            type = MODULUS;
            lexeme[0] = nextCharacter; lexeme[1] = '\0';
            reader.GetNextCharacter();
            break;
         case '^': 
            type = POWER;
            lexeme[0] = nextCharacter; lexeme[1] = '\0';
            reader.GetNextCharacter();
            break;
         default:  
            type = UNKTOKEN;
            lexeme[0] = nextCharacter; lexeme[1] = '\0';
            reader.GetNextCharacter();
            break;
      }
   }

   tokens[LOOKAHEAD].type = type;
   strcpy(tokens[LOOKAHEAD].lexeme,lexeme);
   tokens[LOOKAHEAD].sourceLineNumber = sourceLineNumber;
   tokens[LOOKAHEAD].sourceLineIndex = sourceLineIndex;

#ifdef TRACESCANNER
   sprintf(information,"At (%4d:%3d) token = %12s lexeme = |%s|",
      tokens[LOOKAHEAD].sourceLineNumber,
      tokens[LOOKAHEAD].sourceLineIndex,
      TokenDescription(type),lexeme);
   lister.ListInformationLine(information);
#endif

}

//-----------------------------------------------------------
const char *TokenDescription(TOKENTYPE type)
//-----------------------------------------------------------
{
   int i;
   bool isFound;
   
   isFound = false;
   i = 0;
   while ( !isFound && (i <= (sizeof(TOKENTABLE)/sizeof(TOKENTABLERECORD))-1) )
   {
      if ( TOKENTABLE[i].type == type )
         isFound = true;
      else
         i++;
   }
   return ( isFound ? TOKENTABLE[i].description : "???????" );
}
