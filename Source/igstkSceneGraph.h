/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSceneGraph.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSceneGraph_h
#define __igstkSceneGraph_h

#include <iostream>
#include "igstkCoordinateSystemSetTransformResult.h"
#include "igstkSceneGraphNode.h"
#include "igstkCoordinateSystem.h"
#include <string>
#include "igstkSceneGraphUI.h"
#include "igstkCoordinateSystemTransformToResult.h"

namespace igstk 
{

  /** \class SceneGraph
 * 
 * \brief This class represents the Scene Graph of the application.
 *
 * The Scene Graph class will capture the relationships between 
 * coordinate systems created in the application and will create
 * a data structure to hold all the relations. It also provides
 * functionalities to export the tree structure in .dot format and 
 * see the structure dynamically in an FLTK window.
 *
 * http://en.wikipedia.org/wiki/Scene_graph
 *
 */
class SceneGraph
{

public:

  /**  Function to add the coordinate system relationships 
  * to the scene graph.
  */
  void AddCoordinateSystem(const 
             CoordinateSystemSetTransformEvent* setTransformEvent);

  /**  Function to get a new instance of the scene graph. Singleton pattern.
  */
  static SceneGraph* getInstance();

  /** Function to print the node details to std::cout. 
  */
  void printNodeDetails();

  /**  Function to detach the coordinate system.
  */
  void DetachCoordinateSystem(const 
          CoordinateSystemSetTransformEvent* setTransformEvent);

  /**  Funtion to export the scene graph to dot format.
  */
  void ExportSceneGraphToDot(std::string fileName);

  /**  Function to show the scene graph in a new FLTK window.
  */
  void ShowSceneGraph(bool showFlag);

  /**  Function to Show the transfom path for the given transform event.
  */
  void ShowTheTransformPath(const 
          CoordinateSystemTransformToEvent* transformPathEvent);

  /**  Funtion to set the node selected when the user selects of deselects it.
  */
  void SetNodeDetails(const char* nodeName, bool includeFlag);

private:
  /** Destructor. 
   */
  ~SceneGraph();

   /** The main data structure of the scene graph class. 
   */
   std::list<SceneGraphNode*> rootNodes;

   /** The data structure to store all the wanted transitions. 
   */
   std::list<Transition *> transitionsStore;

   /** flag to check if the UI window is open. 
   */
    bool isUIBeingShown;

   /** Function to check if the parent is existing, when a new relationship
   *  is created.
   */
  bool IsParentExisting(SceneGraphNode* parentNode, const char *parentName);

  /**  Method to add the coordinate system if the parent exists.
   */
  void AddCoordinateSystemToParent(SceneGraphNode* parentNode, 
    const CoordinateSystemSetTransformEvent *setTransformEvent);
  
  /**  Function to create a new parent and add the corresponding 
  * nodes when the parent in not existing.
   */
  void CreateNewParentAndAddCoordinateSystem(const 
       CoordinateSystemSetTransformEvent *setTransformEvent);

  /**  Flag to check if the singleton class is created.
   */
  static bool instanceFlag;

  /**  The variable to hold singleton class.
   */
    static SceneGraph *single;

  /**  Constructor.
   */
  SceneGraph();

  /**  Function to print the details of the given parent.
   */
  void printParentNodeDetails(SceneGraphNode* parentNode);

  /**  Function to print the details of the given node.
   */
  void print(SceneGraphNode* node);

  /**  Fucntion to export the description to dot to the given
  * stream.
   */
  void ExportDescriptionToDot(std::ostream & ostr);

  /**  Function to draw the parent child relationship for .dot 
  * export functionality.
   */
  void DrawParentChildrenRelationship(std::ostream &ostr,  
    igstk::SceneGraphNode *parentNode);

  /**  Function to draw the description of each node to the stream given.
   */
  void DrawDescription(std::ostream &ostr,  
    igstk::SceneGraphNode *parentNode);

  /**  The UI instance pointer. 
   */
  SceneGraphUI* m_SceneGraphUI;

  /**  Function to Optimize the tree structure.
   */
  void OptimizeTree();

  /**  Function called by OptimizeTree() to club if the node
  * is duplicated.
   */
  bool ClubBothRootNodes(SceneGraphNode* rootNode, 
    SceneGraphNode* childNode);

  /**  Reset the CurrentTranform Flag for the all the nodes under
  * the parent Node given.
   */
  void ResetCurrentTransformFlag(SceneGraphNode* parentNode);

  /**  Function to set the transform path. This function calculates the path
  * from ancestor to the child and sets inverse if the tranform in inverse.
  */
  void SetTransformPath(const char * ancestorName,
    const char * childName, bool isInverse);

  /**  Function to set the tranformflag of the common ancestor found and the child.
  */
  bool CheckAndSetTransformFlag(SceneGraphNode* ancestorNode,
    const char * childName, bool isInverse);

  /**  Function to get the reference to the scene graph node with 
  * the given name.
  */
  SceneGraphNode* LocateSceneGraphNodeWithName(const char * name);

  /** Function to get the reference to the scene graph in the parent node 
  * with the given name. This function is called internally by 
  * LocateSceneGraphNodeWithName() function.
  */
  SceneGraphNode* LocateSceneGraphNodeInRootNode(SceneGraphNode* parentNode,
    const char * name);

  /**  Function to check if the user selected node is there in the 
  * transform event.
  */
  bool IsRequestedItemThere(const 
    CoordinateSystemTransformToEvent *setTransformEvent);

  /**  Function called by IsRequestedItemThere() to check if the requested 
  * item is there or not.
  */
  bool CheckRequestedItemThere(SceneGraphNode* node,
    const CoordinateSystemTransformToEvent *setTransformEvent);

  /** Function called by CheckRequestedItemThere() to check if the tranform 
  * event is there in the source or destination.
  */
  bool CompareEventSourceAndDestination(SceneGraphNode* node,
    const CoordinateSystemTransformToEvent *setTransformEvent);
  
  /**  Function to check if the tranform event is a new one or one among 
  * already captured events.
  */
  bool IsNewTranformEvent(const 
    CoordinateSystemTransformToEvent *setTransformEvent);
};
} //end namespace igstk.
#endif