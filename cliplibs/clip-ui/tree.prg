/*-------------------------------------------------------------------------*/
/*   This is a part of CLIP-UI library                                     */
/*                                                                         */
/*   Copyright (C) 2003 by E/AS Software Foundation                        */
/*   Author: Igor Satsyuk <satsyuk@tut.by>                                 */
/*   Last change: 09 Jun 2004                                              */
/*                                                                         */
/*   This program is free software; you can redistribute it and/or modify  */
/*   it under the terms of the GNU General Public License as               */
/*   published by the Free Software Foundation; either version 2 of the    */
/*   License, or (at your option) any later version.                       */
/*-------------------------------------------------------------------------*/
#include "clip-ui.ch"

static driver := getDriver()

/* 
TODO: id by data, not index, nice expand/collapse icons, possible remove lines, ignore expand/collapse as node activation on single LMB click, fill tree from object 
*/

/* Tree class */
function UITree(nTreeColumn, acNameColumns)
	local obj := driver:createTree(nTreeColumn, acNameColumns)
	obj:className 	:= "UITree"
	obj:nodes 	:= array(0)
	obj:nodesId 	:= array(0)
	obj:onSelect 	:= NIL
	_recover_UITREE(obj)
return obj

function _recover_UITREE( obj )
	obj:addNode 	:= @ui_addNode()
	obj:setAction 	:= @ui_setAction()
	obj:clear 	:= @ui_clear()
	obj:getSelection := @ui_getSelection()
	obj:getSelectionId := @ui_getSelectionId()
	//obj:openView := @ui_openView()
return obj

/* Add node and fill it by data */
static function ui_addNode(self, columns, id, parent, sibling, expanded)
	local i, node
	// Convert values to strings
	for i=1 to len(columns)
	      	columns[i] := val2str(columns[i])
	next
	expanded := iif(valtype(expanded)=="U",.T.,expanded)
	node := driver:addTreeNode(self, parent, sibling, columns, expanded)
	aadd(self:nodes, node)
	aadd(self:nodesId, id)
return node

/* Set action to UITree */
static function ui_setAction(self, action)
  	// Set action to single-click LMB or key ENTER pressed
	self:onSelect := action
	driver:setTreeSelectAction( self, {|w,e| treeNodeSelect(self, e)} )
return NIL

/* Slot for tree selection */
function treeNodeSelect(tree, event)
  	if driver:conditionTreeSelection(tree, event)
     		eval(tree:onSelect, tree, tree:getSelection(event))
	endif
return

/* Clear all nodes */
static function ui_clear(self)
	driver:clearTree( self )
	self:nodes := array(0)
	self:nodesId := array(0)
return NIL

/* Get current selection */
static function ui_getSelection(self)
return driver:getTreeSelection( self )

/* Get current selection ID */
static function ui_getSelectionId(self)
	local sel
	sel := driver:getTreeSelection( self )
return iif(sel<=0 .or. sel>len(self:nodesId), NIL, self:nodesId[sel] )