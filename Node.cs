using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Node : MonoBehaviour
{

    public Node[] neighbours;
    public Vector2[] validDirections;


    // Use this for initialization
    void Start()
    {
        //initialize validDirection
        validDirections = new Vector2[neighbours.Length];
        for (int i=0; i<neighbours.Length; i++)
        {
            //create a temp var to store the nodes
            Node neighbour = neighbours[i];

            //check our possible directions     **!!!!!!!!Change for MEGA UNIT!!!!!!!
            Vector2 tempVector = neighbour.transform.localPosition - transform.localPosition;

            //store the normalized value
            validDirections[i] = tempVector.normalized;     //!!!!!!MEGA!!!!!!!

        }
    }
}
