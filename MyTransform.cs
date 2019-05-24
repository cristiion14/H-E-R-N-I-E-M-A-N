using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteInEditMode]
public class MyTransform : MonoBehaviour
{

    public Vector3 position;
    public Vector3 rotation;
    public Vector3 scale = new Vector3(1, 1, 1);
    Vector3[] ModelSpaceVert;
    VectorMaths.BoundingCircle boundingCircle;
    VectorMaths.BoundingCircle boundingCircle2;
    public float yawAngle;

    GameObject sphere;
    GameObject cube;
    GameObject cylinder;

    Vector3[] cubeSpaceVert;
    Vector3[] sphereSpaceVert;
    Vector3[] cylinderSpaceVert;
    public Mesh mesh;
    void Start()
    {
        
        //get mesh
        MeshFilter MF = GetComponent<MeshFilter>();
        Mesh tempMesh = Mesh.Instantiate(mesh) as Mesh;
        MF.sharedMesh = tempMesh;
        ModelSpaceVert = MF.sharedMesh.vertices;


    }

    void Update()
    {
        //yawAngle += Time.deltaTime;

        //  Matrix Transf
        Vector3[] TransfVert = new Vector3[ModelSpaceVert.Length];

        //translation matrix
        VectorMaths.Matrix4by4 translationMat = new VectorMaths.Matrix4by4(
        new Vector3(1, 0, 0),
        new Vector3(0, 1, 0),
        new Vector3(0, 0, 1),
        new Vector3(position.x, position.y, position.z));

        //scale matrix
        VectorMaths.Matrix4by4 scaleMatrix = new VectorMaths.Matrix4by4(new Vector3(scale.x, 0, 0) , new Vector3(0, scale.y, 0), new Vector3(0, 0, scale.z), Vector3.zero);

        //roll rot matrix
        VectorMaths.Matrix4by4 rollMatrix = new VectorMaths.Matrix4by4(
               new Vector3(Mathf.Cos(rotation.z), Mathf.Sin(rotation.z), 0),
               new Vector3(-Mathf.Sin(rotation.z), Mathf.Cos(rotation.z), 0),
               new Vector3(0, 0, 1),
               Vector3.zero);

        //pitch rot matrix
        VectorMaths.Matrix4by4 pitchMatrix = new VectorMaths.Matrix4by4(
            new Vector3(1, 0, 0),
            new Vector3(0, Mathf.Cos(rotation.x), Mathf.Sin(rotation.x)),
            new Vector3(0, -Mathf.Sin(rotation.x), Mathf.Cos(rotation.x)),
            Vector3.zero);

        //yaw rot matrix
        VectorMaths.Matrix4by4 yawMatrix = new VectorMaths.Matrix4by4(
            new Vector3(Mathf.Cos(rotation.y), 0, -Mathf.Sin(rotation.y)),
            new Vector3(0, 1, 0),
            new Vector3(Mathf.Sin(rotation.y), 0, Mathf.Cos(rotation.y)),
            Vector3.zero);

        VectorMaths.Matrix4by4 R = yawMatrix * (pitchMatrix * rollMatrix);
        VectorMaths.Matrix4by4 M = translationMat * (R * scaleMatrix);

        //Apply transformation for sphere (S  R  T)
        for (int i = 0; i < TransfVert.Length; i++)
        {
            /*
            Vector3 RolledVertex = rollMatrix * ModelSpaceVert[i];
            Vector3 PitchedVertex = pitchMatrix * RolledVertex;
            Vector3 YawedVertex = yawMatrix * PitchedVertex;
            //  TransfVert[i] = translationMat * ModelSpaceVert[i];
            */
           TransfVert[i] = M * ModelSpaceVert[i];
        }

        //Recalculate mesh
        MeshFilter MF = GetComponent<MeshFilter>();

        MF.sharedMesh.vertices = TransfVert;
        MF.sharedMesh.RecalculateNormals();
        MF.sharedMesh.RecalculateBounds();

    
    }
}