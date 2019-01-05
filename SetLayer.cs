using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
public class SetLayer : NetworkBehaviour
{
    [SerializeField]
    private GameObject Body;
    private GameObject Body1;
    private string playerLayerName = "Body";
    private string playerLayerName1 = "Body1";
    void Start()
    {
        Body = GameObject.Find("Body");
        Body1 = GameObject.Find("Body");
        if(!isLocalPlayer)
        {
            Util.SetLayerRecursively(Body, LayerMask.NameToLayer(playerLayerName));
        }
        if(isLocalPlayer)
        {
            Util.SetLayerRecursively(Body1, LayerMask.NameToLayer(playerLayerName1));
        }
    }
   
}