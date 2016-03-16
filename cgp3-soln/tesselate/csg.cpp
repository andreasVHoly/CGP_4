//
// csg
//

#include "csg.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <limits>
#include <stack>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace std;
// using namespace cgp;

GLfloat defaultCol[] = {0.243f, 0.176f, 0.75f, 1.0f};

bool Scene::genVizRender(View * view, ShapeDrawData &sdd)
{
    std::vector<ShapeNode *> leaves;
    std::stack<SceneNode *> nodes;
    SceneNode * currnode;
    OpNode * currop;
    ShapeNode * currshape;
    int i;

    geom.clear();
    geom.setColour(defaultCol);

    // gather vector of leaf nodes
    if(csgroot != NULL)
    {
        if(dynamic_cast<ShapeNode*>( csgroot ) == 0) // OpNode
        {
            nodes.push(csgroot);
        }
        else // ShapeNode
        {
            currshape = dynamic_cast<ShapeNode*>( csgroot );
            leaves.push_back((ShapeNode *) csgroot);
        }

        // explicit walk of csg tree using stack
        while(!nodes.empty())
        {
            currnode = nodes.top();
            currop = dynamic_cast<OpNode*> (currnode);
            nodes.pop();

            // left subtree
            if(dynamic_cast<ShapeNode*>( currop->left ) == 0) // cast fails so it is an OpNode
            {
                nodes.push(currop->left);
            }
            else // ShapeNode, so treat as leaf
            {
                currshape = dynamic_cast<ShapeNode*>( currop->left );
                leaves.push_back(currshape);
            }

            // right subtree
            if(dynamic_cast<ShapeNode*>( currop->right ) == 0) // cast fails so it is an OpNode
            {
                nodes.push(currop->right);
            }
            else // ShapeNode, so treat as leaf
            {
                currshape = dynamic_cast<ShapeNode*>( currop->right );
                leaves.push_back(currshape);
            }
        }
    }

    // traverse leaf shapes generating geometry
    for(i = 0; i < (int) leaves.size(); i++)
    {
        leaves[i]->shape->genGeometry(&geom, view);
    }

    // bind geometry to buffers and return drawing parameters, if possible
    if(geom.bindBuffers(view))
    {
        sdd = geom.getDrawParameters();
        return true;
    }
    else
        return false;
}

bool Scene::genVoxRender(View * view, ShapeDrawData &sdd)
{
    int x, y, z, xdim, ydim, zdim;
    glm::mat4 tfm, idt;
    glm::vec3 trs;
    cgp::Point pnt;

    geom.clear();
    geom.setColour(defaultCol);

    if(rep == SceneRep::VOXELS)
    {
        idt = glm::mat4(1.0f); // identity matrix

        vox.getDim(xdim, ydim, zdim);

        // place a sphere at filled voxels but subsample to avoid generating too many spheres
        for(x = 0; x < xdim; x+=10)
            for(y = 0; y < ydim; y+=10)
                for(z = 0; z < zdim; z+=10)
                {
                    if(vox.get(x, y, z))
                    {
                        pnt = vox.getVoxelPos(x, y, z); // convert from voxel space to world coordinates
                        trs = glm::vec3(pnt.x, pnt.y, pnt.z);
                        tfm = glm::translate(idt, trs);
                        geom.genSphere(voxsidelen * 5.0f, 3, 3, tfm);
                    }
                }

    }

    // bind geometry to buffers and return drawing parameters, if possible
    if(geom.bindBuffers(view))
    {
        sdd = geom.getDrawParameters();
        return true;
    }
    else
        return false;
}

Scene::Scene()
{
    csgroot = NULL;
    col = defaultCol;
    voldiag = cgp::Vector(20.0f, 20.0f, 20.0f);
    voxsidelen = 0.0f;
    rep = SceneRep::TREE;
}

Scene::~Scene()
{
    clear();
}

void Scene::clear()
{
    std::stack<SceneNode *> nodes;
    SceneNode * currnode;
    OpNode * currop;
    ShapeNode * currleaf;

    geom.clear();
    vox.clear();

    // walk csg tree and deallocate nodes
    if(csgroot != NULL)
    {
        nodes.push(csgroot);

        // explicit walk of csg tree using stack
        while(!nodes.empty())
        {
            currnode = nodes.top();
            nodes.pop(); // calls destructor

            if(dynamic_cast<OpNode*> (currnode))
            {
                currop = dynamic_cast<OpNode*> (currnode);
                nodes.push(currop->right);
                nodes.push(currop->left);
                delete currop;
            }
            else
            {
                if(dynamic_cast<ShapeNode*> (currnode))
                {
                    currleaf = dynamic_cast<ShapeNode*> (currnode);
                    delete currleaf;
                }
                else
                {
                    cerr << "Error Scene::clear(): CSG tree is not properly formed" << endl;
                }
            }
        }
    }
}

bool Scene::bindGeometry(View * view, ShapeDrawData &sdd)
{
    bool pass;

    // call different geometric construction methods depending on current representation
    switch(rep)
    {
        case SceneRep::TREE:
            pass = genVizRender(view, sdd);
            break;
        case SceneRep::VOXELS:
            pass = genVoxRender(view, sdd);
            break;
        case SceneRep::ISOSURFACE:
            pass = voxmesh.bindGeometry(view, sdd);
            break;
        default:
            pass = false;
            break;
    }
    return pass;
}

void Scene::voxSetOp(SetOp op, VoxelVolume *leftarg, VoxelVolume *rightarg)
{
   /*
    switch based on op
    UNION: wherever voxel is set in rightarg copy to leftarg
    INTERSECTION: if voxel is set in leftarg, check to see if it is also set in rightarg, otherwise switch it off
    DIFFERENCE: wherever voxel is set in rightarg turn it off in leftarg
    */

    int x, y, z, dx1, dx2, dy1, dy2, dz1, dz2;

    // check that volume sizes match
    leftarg->getDim(dx1, dy1, dz1);
    rightarg->getDim(dx2, dy2, dz2);
    if(dx1 != dx2 || dy1 != dy2 || dz1 != dz2)
    {
    }
    else
    {
        switch(op)
        {
            case SetOp::UNION: // wherever voxel is set in rightarg copy to leftarg
                for(x = 0; x < dx1; x++)
                    for(y = 0; y < dy1; y++)
                        for(z = 0; z < dz1; z++)
                            if(rightarg->get(x,y,z))
                                leftarg->set(x,y,z,true);
                break;
            case SetOp::INTERSECTION: // if voxel is set in leftarg, check to see if it is also set in rightarg, otherwise switch it off
                for(x = 0; x < dx1; x++)
                    for(y = 0; y < dy1; y++)
                        for(z = 0; z < dz1; z++)
                             if(leftarg->get(x,y,z))
                                 if(!rightarg->get(x,y,z))
                                     leftarg->set(x,y,z,false);
                break;
            case SetOp::DIFFERENCE: // wherever voxel is set in rightarg turn it off in leftarg
                for(x = 0; x < dx1; x++)
                    for(y = 0; y < dy1; y++)
                        for(z = 0; z < dz1; z++)
                            if(rightarg->get(x,y,z))
                                leftarg->set(x,y,z,false);
                break;
            default:
                break;
        }
    }
}

void Scene::voxWalk(SceneNode *root, VoxelVolume *voxels)
{
    // traverse csg tree by depth first recursive walk
    /*
    if(root is leaf)
        convert to voxel rep, store in voxels
    else
        voxWalk left tree (leftnode, voxels)
        allocate rightvoxels, copy parameters from voxels
        voxWalk right tree (rightnode, rightvoxels)
        apply opp to voxels and rightvoxels store results in voxels
        deallocate rightvoxels
    */

    VoxelVolume * rightvoxels;
    ShapeNode * shapenode;
    OpNode * opnode;
    int dx, dy, dz;
    cgp::Point o;
    cgp::Vector d;

    if(dynamic_cast<ShapeNode*>( root )) // SceneNode
    {
        shapenode = dynamic_cast<ShapeNode*>( root );
        // needs to be improved by only considering bounding box area
        voxels->getDim(dx, dy, dz);

        cerr << "xdim = " << dx << endl;
        for(int x = 0; x < dx; x++)
        {
            #pragma omp parallel for
            for(int y = 0; y < dy; y++)
                for(int z = 0; z < dz; z++)
                    voxels->set(x,y,z, shapenode->shape->pointContainment(voxels->getVoxelPos(x,y,z)));
            // /cerr << "point containment slice x = " << x << endl;
        }
    }
    else // OpNode
    {
        opnode = dynamic_cast<OpNode*>( root );
        voxWalk(opnode->left, voxels);
        voxels->getDim(dx, dy, dz);
        voxels->getFrame(o, d);
        rightvoxels = new VoxelVolume(dx, dy, dz, o, d);
        voxWalk(opnode->right, rightvoxels);
        voxSetOp(opnode->op, voxels, rightvoxels);
        delete rightvoxels;
    }
}

void Scene::voxelise(float voxlen)
{
    int xdim, ydim, zdim;

    // calculate voxel volume dimensions based on voxlen
    xdim = ceil(voldiag.i / voxlen)+2; // needs a 1 voxel border to ensure a closed mesh
    ydim = ceil(voldiag.j / voxlen)+2;
    zdim = ceil(voldiag.k / voxlen)+2;
    voxsidelen = voxlen;

    cgp::Vector voxdiag = cgp::Vector((float) xdim * voxlen, (float) ydim * voxlen, (float) zdim * voxlen);
    cgp::Point voxorigin = cgp::Point(-0.5f*voxdiag.i, -0.5f*voxdiag.j, -0.5f*voxdiag.k);
    vox.setDim(xdim, ydim, zdim);
    vox.setFrame(voxorigin, voxdiag);

    // cerr << "Voxel volume dimensions = " << xdim << " x " << ydim << " x " << zdim << endl;

    // actual recursive depth-first walk of csg tree
    if(csgroot != NULL)
        voxWalk(csgroot, &vox);
    rep = SceneRep::VOXELS;
}

void Scene::isoextract()
{
    voxmesh.marchingCubes(&vox);
    rep = SceneRep::ISOSURFACE;
}

void Scene::smooth()
{
    voxmesh.laplacianSmooth(6, 1.0f);
}

void Scene::deform(ffd * def)
{
    voxmesh.applyFFD(def);
}

void Scene::sampleScene()
{
    ShapeNode * sph = new ShapeNode();
    sph->shape = new Sphere(cgp::Point(0.0f, 0.0f, 0.0f), 4.0f);

    ShapeNode * cyl1 = new ShapeNode();
    cyl1->shape = new Cylinder(cgp::Point(-7.0f, -7.0f, 0.0f), cgp::Point(7.0f, 7.0f, 0.0f), 2.0f);

    ShapeNode * cyl2 = new ShapeNode();
    cyl2->shape = new Cylinder(cgp::Point(0.0f, -7.0f, 0.0f), cgp::Point(0.0f, 7.0f, 0.0f), 2.5f);

    OpNode * combine = new OpNode();
    combine->op = SetOp::UNION;
    combine->left = sph;
    combine->right = cyl1;

    OpNode * diff = new OpNode();
    diff->op = SetOp::DIFFERENCE;
    diff->left = combine;
    diff->right = cyl2;

    csgroot = diff;
}

void Scene::expensiveScene()
{
    ShapeNode * sph = new ShapeNode();
    sph->shape = new Sphere(cgp::Point(1.0f, -2.0f, -2.0f), 3.0f);

    ShapeNode * cyl = new ShapeNode();
    cyl->shape = new Cylinder(cgp::Point(-7.0f, -7.0f, 0.0f), cgp::Point(7.0f, 7.0f, 0.0f), 2.0f);

    ShapeNode * mesh = new ShapeNode();
    Mesh * bunny = new Mesh();
    bunny->readSTL("../meshes/cube.stl");
    bunny->boxFit(10.0f);
    mesh->shape = bunny;

    OpNode * combine = new OpNode();
    combine->op = SetOp::UNION;
    combine->left = mesh;
    combine->right = cyl;

    csgroot = combine;
}




void Scene::freddyScene(){


    //LEFT FOOT//
    //3 speheres and 3 cylinders
    ShapeNode * footSph1_l = new ShapeNode();
    footSph1_l->shape = new Sphere(cgp::Point(1.8f, -2.4f, 2.9f), 0.3f);

    ShapeNode * footSph2_l = new ShapeNode();
    footSph2_l->shape = new Sphere(cgp::Point(1.2f, -2.4, 3.0f), 0.3f);

    ShapeNode * footSph3_l = new ShapeNode();
    footSph3_l->shape = new Sphere(cgp::Point(0.6f, -2.4f, 3.1f), 0.3f);

    ShapeNode * footCyl1_l = new ShapeNode();
    footCyl1_l->shape = new Cylinder(cgp::Point(1.2f, -2.1f, 2.1f), cgp::Point(1.8f, -2.4f, 2.9f), 0.3f);

    ShapeNode * footCyl2_l = new ShapeNode();
    footCyl2_l->shape = new Cylinder(cgp::Point(0.8f, -2.1f, 2.2f), cgp::Point(1.2f, -2.4f, 3.0f), 0.3f);

    ShapeNode * footCyl3_l = new ShapeNode();
    footCyl3_l->shape = new Cylinder(cgp::Point(0.4f, -2.1f, 2.3f), cgp::Point(0.6f, -2.4f, 3.1f), 0.3f);


    OpNode * foot_left_4 = new OpNode();
    foot_left_4->op = SetOp::UNION;
    foot_left_4->left = footSph3_l;
    foot_left_4->right = footCyl3_l;

    OpNode * foot_left_2 = new OpNode();
    foot_left_2->op = SetOp::UNION;
    foot_left_2->left = footSph1_l;
    foot_left_2->right = footCyl1_l;

    OpNode * foot_left_3 = new OpNode();
    foot_left_3->op = SetOp::UNION;
    foot_left_3->left = footSph2_l;
    foot_left_3->right = footCyl2_l;

    OpNode * foot_left_1 = new OpNode();
    foot_left_1->op = SetOp::UNION;
    foot_left_1->left = foot_left_3;
    foot_left_1->right = foot_left_4;

    OpNode * foot_left = new OpNode();
    foot_left->op = SetOp::UNION;
    foot_left->left = foot_left_1;
    foot_left->right = foot_left_2;


    //RIGHT FOOT//
    //3 speheres and 3 cylinders
    ShapeNode * footSph1_r = new ShapeNode();
    footSph1_r->shape = new Sphere(cgp::Point(-1.8f, -2.4f, 2.9f), 0.3f);

    ShapeNode * footSph2_r = new ShapeNode();
    footSph2_r->shape = new Sphere(cgp::Point(-1.2f, -2.4f, 3.0f), 0.3f);

    ShapeNode * footSph3_r = new ShapeNode();
    footSph3_r->shape = new Sphere(cgp::Point(-0.6f, -2.4f, 3.1f), 0.3f);

    ShapeNode * footCyl1_r = new ShapeNode();
    footCyl1_r->shape = new Cylinder(cgp::Point(-1.2f, -2.1f, 2.1f), cgp::Point(-1.8f, -2.4f, 2.9f), 0.3f);

    ShapeNode * footCyl2_r = new ShapeNode();
    footCyl2_r->shape = new Cylinder(cgp::Point(-0.8f, -2.1f, 2.2f), cgp::Point(-1.2f, -2.4f, 3.0f), 0.3f);

    ShapeNode * footCyl3_r = new ShapeNode();
    footCyl3_r->shape = new Cylinder(cgp::Point(-0.4f, -2.1f, 2.3f), cgp::Point(-0.6f, -2.4f, 3.1f), 0.3f);


    OpNode * foot_right_4 = new OpNode();
    foot_right_4->op = SetOp::UNION;
    foot_right_4->left = footSph3_r;
    foot_right_4->right = footCyl3_r;

    OpNode * foot_right_3 = new OpNode();
    foot_right_3->op = SetOp::UNION;
    foot_right_3->left = footSph2_r;
    foot_right_3->right = footCyl2_r;

    OpNode * foot_right_2 = new OpNode();
    foot_right_2->op = SetOp::UNION;
    foot_right_2->left = footSph1_r;
    foot_right_2->right = footCyl1_r;



    OpNode * foot_right_1 = new OpNode();
    foot_right_1->op = SetOp::UNION;
    foot_right_1->left = foot_right_3;
    foot_right_1->right = foot_right_4;


    OpNode * foot_right = new OpNode();
    foot_right->op = SetOp::UNION;
    foot_right->left = foot_right_1;
    foot_right->right = foot_right_2;

    /*foot_right->left = footSph1_r;
    foot_right->right = footCyl1_r;*/


    //LEFT HAND//
    //made up of a spheres and 3 cylinders

    //palm
    ShapeNode * handSph_l = new ShapeNode();
    handSph_l->shape = new Sphere(cgp::Point(-1.8f, -0.7f, 1.0f), 1.5f);
    //finger1

    //finger1
    ShapeNode * handCyl2_l = new ShapeNode();
    handCyl2_l->shape = new Cylinder(cgp::Point(-1.2f, -1.2f, 2.5f), cgp::Point(-2.7f, -0.8f, 1.9f), 0.25f);
    //finger1
    ShapeNode * handCyl3_l = new ShapeNode();
    handCyl3_l->shape = new Cylinder(cgp::Point(-0.75f, -1.6f, 2.5f), cgp::Point(-2.5f, -1.2f, 1.9f), 0.25f);


    OpNode * hand_left_1 = new OpNode();
    hand_left_1->op = SetOp::UNION;
    hand_left_1->left = handSph_l;
    hand_left_1->right = handCyl2_l;

  /*  OpNode * hand_left_2 = new OpNode();
    hand_left_2->op = SetOp::UNION;
    hand_left_2->left = handCyl3_l;
    hand_left_2->right = handCyl2_l;*/

    OpNode * hand_left = new OpNode();
    hand_left->op = SetOp::UNION;
    hand_left->left = hand_left_1;
    hand_left->right = handCyl3_l;





    //RIGHT HAND//
    //made up of a spheres and 3 cylinders

    //palm
    ShapeNode * handSph_r = new ShapeNode();
    handSph_r->shape = new Sphere(cgp::Point(1.8f, -0.7f, 1.0f), 1.5f);

    //finger1
    ShapeNode * handCyl2_r = new ShapeNode();
    handCyl2_r->shape = new Cylinder(cgp::Point(1.2f, -1.2f, 2.5f), cgp::Point(2.7f, -0.8f, 1.9f), 0.25f);
    //finger1
    ShapeNode * handCyl3_r = new ShapeNode();
    handCyl3_r->shape = new Cylinder(cgp::Point(0.75f, -1.6f, 2.5f), cgp::Point(2.5f, -1.2f, 1.9f), 0.25f);


    OpNode * hand_right_1 = new OpNode();
    hand_right_1->op = SetOp::UNION;
    hand_right_1->left = handSph_r;
    hand_right_1->right = handCyl2_r;

    /*OpNode * hand_right_2 = new OpNode();
    hand_right_2->op = SetOp::UNION;
    hand_right_2->left = handCyl3_r;
    hand_right_2->right = handCyl2_r;*/

    OpNode * hand_right = new OpNode();
    hand_right->op = SetOp::UNION;
    hand_right->left = hand_right_1;
    hand_right->right = handCyl3_r;






    //NOSE//
    //made up of a cone and a star



    ShapeNode * nose1 = new ShapeNode();
    nose1->shape = new Cylinder(cgp::Point(0.0f, 1.3f, 3.1f), cgp::Point(0.0f, 1.6f, 3.6f), 0.45f);

    ShapeNode * nose2 = new ShapeNode();
    nose2->shape = new Cylinder(cgp::Point(0.0f, 1.6f, 3.5f), cgp::Point(0.0f, 2.1f, 4.4f), 0.36f);

    ShapeNode * nose3 = new ShapeNode();
    nose3->shape = new Cylinder(cgp::Point(0.0f, 2.0f, 4.3f), cgp::Point(0.0f, 2.4f, 5.3f), 0.33f);

    ShapeNode * noseSph1 = new ShapeNode();
    noseSph1->shape = new Sphere(cgp::Point(0.1f, 2.4f, 5.25f), 0.15f);

    ShapeNode * noseSph2 = new ShapeNode();
    noseSph2->shape = new Sphere(cgp::Point(-0.1f, 2.4f, 5.25f), 0.15f);


    OpNode * holes = new OpNode();
    holes->op = SetOp::UNION;
    holes->left = noseSph1;
    holes->right = noseSph2;



    OpNode * noseop1 = new OpNode();
    noseop1->op = SetOp::UNION;
    noseop1->left = nose1;
    noseop1->right = nose2;

    /*OpNode * noseop2 = new OpNode();
    noseop2->op = SetOp::UNION;
    noseop2->left = nose3;
    noseop2->right = nose4;*/


    OpNode * noseop2_1 = new OpNode();
    noseop2_1->op = SetOp::DIFFERENCE;
    noseop2_1->left = nose3;
    noseop2_1->right = holes;

    OpNode * noseop3 = new OpNode();
    noseop3->op = SetOp::UNION;
    noseop3->left = noseop1;
    noseop3->right = noseop2_1;



    /*OpNode * noseop3 = new OpNode();
    noseop3->op = SetOp::UNION;
    noseop3->left = noseop1;
    noseop3->right = nose3;*/




    //HEAD//
    //made up of 3 spheres

    //main head
    ShapeNode * headSph1 = new ShapeNode();
    headSph1->shape = new Sphere(cgp::Point(0.0f, 0.6f, 0.0f), 3.3f);
    //eye2
    ShapeNode * eyeSph1 = new ShapeNode();
    eyeSph1->shape = new Sphere(cgp::Point(-1.2f, 1.3f, 3.1f), 0.3f);
    //eye1
    ShapeNode * eyeSph2 = new ShapeNode();
    eyeSph2->shape = new Sphere(cgp::Point(1.2f, 1.3f, 3.1f), 0.3f);


    OpNode * head_eyes = new OpNode();
    head_eyes->op = SetOp::DIFFERENCE;
    head_eyes->left = headSph1;
    head_eyes->right = eyeSph1;


    OpNode * head = new OpNode();
    head->op = SetOp::DIFFERENCE;
    head->left = head_eyes;
    head->right = eyeSph2;






    //BODY//
    //made up of 3 spheres

    //left fat bubble
    ShapeNode * bodySph1 = new ShapeNode();
    bodySph1->shape = new Sphere(cgp::Point(-0.5f, 0.0f, 0.0f), 3.0f);

    //middle fat bubble
    ShapeNode * bodySph2 = new ShapeNode();
    bodySph2->shape = new Sphere(cgp::Point(0.0f, 0.0f, 0.0f), 3.2f);
    //right fat bubble
    ShapeNode * bodySph3 = new ShapeNode();
    bodySph3->shape = new Sphere(cgp::Point(0.5f, 0.0f, 0.0f), 3.0f);



    OpNode * body_lower = new OpNode();
    body_lower->op = SetOp::UNION;
    body_lower->left = bodySph2;
    body_lower->right = bodySph3;


    OpNode * body = new OpNode();
    body->op = SetOp::UNION;
    body->left = body_lower;
    body->right = bodySph1;






    //TAIL//
    //made up of a cylinder and sphere
    ShapeNode * tailSph = new ShapeNode();
    tailSph->shape = new Sphere(cgp::Point(0.0f, -2.0f, -2.8f), 0.2f);

    ShapeNode * tailCyl = new ShapeNode();
    tailCyl->shape = new Cylinder(cgp::Point(0.0f, -2.0f, -2.3f), cgp::Point(0.0f, -2.0f,-2.8f), 0.2f);

    OpNode * tail = new OpNode();
    tail->op = SetOp::UNION;
    tail->left = tailCyl;
    tail->right = tailSph;


/*
    ShapeNode * mesh = new ShapeNode();
    Mesh * bunny = new Mesh();
    bunny->readSTL("../meshes/bunny.stl");
    bunny->boxFit(10.0f);
    mesh->shape = bunny;*/



    //leaf conncetions
    OpNode * head_nose = new OpNode();
    head_nose->op = SetOp::UNION;
    head_nose->left = head;
    head_nose->right = noseop3;
    //l2_1->right = NULL;

    OpNode * tail_body = new OpNode();
    tail_body->op = SetOp::UNION;
    tail_body->left = tail;
    tail_body->right = body;

    OpNode * feet = new OpNode();
    feet->op = SetOp::UNION;
    feet->left = foot_left;
    feet->right = foot_right;

    OpNode * hands = new OpNode();
    hands->op = SetOp::UNION;
    hands->left = hand_left;
    hands->right = hand_right;

    OpNode * bodies = new OpNode();
    bodies->op = SetOp::UNION;
    bodies->left = head_nose;
    bodies->right = tail_body;
    /*bodies->left = foot_right;
    bodies->right = tail_body;*/

    OpNode * limbs = new OpNode();
    limbs->op = SetOp::UNION;
    limbs->left = feet;
    limbs->right = hands;

    OpNode * top = new OpNode();
    top->op = SetOp::UNION;
    top->left = bodies;
    top->right = limbs;
  /*  top->left = bodies;
    top->right = hands;*/


    //bottom cutter
    ShapeNode * newNode = new ShapeNode();
    newNode->shape = new Cylinder(cgp::Point(0.0f, -2.5f, 0.0f), cgp::Point(0.0f, 5.0f, 0.0f), 8.0f);


    //root
    OpNode * cylcut = new OpNode();
    cylcut->op = SetOp::INTERSECTION;
    cylcut->left = top;
    cylcut->right = newNode;



    csgroot = cylcut;
}
