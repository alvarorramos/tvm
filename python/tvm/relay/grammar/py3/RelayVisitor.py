# Generated from /Users/doobs/Code/repo/sampl/tvm/python/tvm/relay/grammar/Relay.g4 by ANTLR 4.7.2
from antlr4 import *
if __name__ is not None and "." in __name__:
    from .RelayParser import RelayParser
else:
    from RelayParser import RelayParser

# This class defines a complete generic visitor for a parse tree produced by RelayParser.

class RelayVisitor(ParseTreeVisitor):

    # Visit a parse tree produced by RelayParser#prog.
    def visitProg(self, device:RelayParser.ProgContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#generalIdent.
    def visitGeneralIdent(self, device:RelayParser.GeneralIdentContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#globalVar.
    def visitGlobalVar(self, device:RelayParser.GlobalVarContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#localVar.
    def visitLocalVar(self, device:RelayParser.LocalVarContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#graphVar.
    def visitGraphVar(self, device:RelayParser.GraphVarContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#exprList.
    def visitExprList(self, device:RelayParser.ExprListContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#callNoAttr.
    def visitCallNoAttr(self, device:RelayParser.CallNoAttrContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#callWithAttr.
    def visitCallWithAttr(self, device:RelayParser.CallWithAttrContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#funcExpr.
    def visitFuncExpr(self, device:RelayParser.FuncExprContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#metaExpr.
    def visitMetaExpr(self, device:RelayParser.MetaExprContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#match.
    def visitMatch(self, device:RelayParser.MatchContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#tensor.
    def visitTensor(self, device:RelayParser.TensorContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#graph.
    def visitGraph(self, device:RelayParser.GraphContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#identExpr.
    def visitIdentExpr(self, device:RelayParser.IdentExprContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#stringExpr.
    def visitStringExpr(self, device:RelayParser.StringExprContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#call.
    def visitCall(self, device:RelayParser.CallContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#neg.
    def visitNeg(self, device:RelayParser.NegContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#tuple.
    def visitTuple(self, device:RelayParser.TupleContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#paren.
    def visitParen(self, device:RelayParser.ParenContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#scalarExpr.
    def visitScalarExpr(self, device:RelayParser.ScalarExprContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#let.
    def visitLet(self, device:RelayParser.LetContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#projection.
    def visitProjection(self, device:RelayParser.ProjectionContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#ifElse.
    def visitIfElse(self, device:RelayParser.IfElseContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#binOp.
    def visitBinOp(self, device:RelayParser.BinOpContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#func.
    def visitFunc(self, device:RelayParser.FuncContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#funcDefn.
    def visitFuncDefn(self, device:RelayParser.FuncDefnContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#externAdtDefn.
    def visitExternAdtDefn(self, device:RelayParser.ExternAdtDefnContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#adtDefn.
    def visitAdtDefn(self, device:RelayParser.AdtDefnContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#constructorName.
    def visitConstructorName(self, device:RelayParser.ConstructorNameContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#adtConsDefnList.
    def visitAdtConsDefnList(self, device:RelayParser.AdtConsDefnListContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#adtConsDefn.
    def visitAdtConsDefn(self, device:RelayParser.AdtConsDefnContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#matchClauseList.
    def visitMatchClauseList(self, device:RelayParser.MatchClauseListContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#matchClause.
    def visitMatchClause(self, device:RelayParser.MatchClauseContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#matchType.
    def visitMatchType(self, device:RelayParser.MatchTypeContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#patternList.
    def visitPatternList(self, device:RelayParser.PatternListContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#wildcardPattern.
    def visitWildcardPattern(self, device:RelayParser.WildcardPatternContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#varPattern.
    def visitVarPattern(self, device:RelayParser.VarPatternContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#constructorPattern.
    def visitConstructorPattern(self, device:RelayParser.ConstructorPatternContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#tuplePattern.
    def visitTuplePattern(self, device:RelayParser.TuplePatternContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#adtCons.
    def visitAdtCons(self, device:RelayParser.AdtConsContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#adtConsParamList.
    def visitAdtConsParamList(self, device:RelayParser.AdtConsParamListContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#adtConsParam.
    def visitAdtConsParam(self, device:RelayParser.AdtConsParamContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#argNoAttr.
    def visitArgNoAttr(self, device:RelayParser.ArgNoAttrContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#argWithAttr.
    def visitArgWithAttr(self, device:RelayParser.ArgWithAttrContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#varList.
    def visitVarList(self, device:RelayParser.VarListContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#var.
    def visitVar(self, device:RelayParser.VarContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#attrSeq.
    def visitAttrSeq(self, device:RelayParser.AttrSeqContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#attr.
    def visitAttr(self, device:RelayParser.AttrContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#tupleType.
    def visitTupleType(self, device:RelayParser.TupleTypeContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#typeParen.
    def visitTypeParen(self, device:RelayParser.TypeParenContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#typeCallType.
    def visitTypeCallType(self, device:RelayParser.TypeCallTypeContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#typeIdentType.
    def visitTypeIdentType(self, device:RelayParser.TypeIdentTypeContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#tensorType.
    def visitTensorType(self, device:RelayParser.TensorTypeContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#funcType.
    def visitFuncType(self, device:RelayParser.FuncTypeContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#incompleteType.
    def visitIncompleteType(self, device:RelayParser.IncompleteTypeContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#typeParamList.
    def visitTypeParamList(self, device:RelayParser.TypeParamListContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#shapeList.
    def visitShapeList(self, device:RelayParser.ShapeListContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#meta.
    def visitMeta(self, device:RelayParser.MetaContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#metaShape.
    def visitMetaShape(self, device:RelayParser.MetaShapeContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#parensShape.
    def visitParensShape(self, device:RelayParser.ParensShapeContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#intShape.
    def visitIntShape(self, device:RelayParser.IntShapeContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#body.
    def visitBody(self, device:RelayParser.BodyContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#scalarFloat.
    def visitScalarFloat(self, device:RelayParser.ScalarFloatContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#scalarInt.
    def visitScalarInt(self, device:RelayParser.ScalarIntContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#scalarBool.
    def visitScalarBool(self, device:RelayParser.ScalarBoolContext):
        return self.visitChildren(device)


    # Visit a parse tree produced by RelayParser#ident.
    def visitIdent(self, device:RelayParser.IdentContext):
        return self.visitChildren(device)



del RelayParser