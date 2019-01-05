using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class AnimationController : NetworkBehaviour {
    Animator anim;
    int jumpHash = Animator.StringToHash("Jump");
    int walkHash = Animator.StringToHash("WalkLeft");
    int notwalkHash = Animator.StringToHash("notWalking");
    int notjumpHash = Animator.StringToHash("noJump");
    int deathHash = Animator.StringToHash("Death");
	// Use this for initialization
  
    void Start()
    {
        
        anim = GetComponent<Animator>();

    }
    void Update()
    {
        if (!isLocalPlayer)
            return;

        float move= Input.GetAxis("Vertical");
        float moveH = Input.GetAxis("Horizontal");
        anim.SetFloat("Speed", move);
        anim.SetFloat("sideSpeed", moveH);
        if(Input.GetKeyDown(KeyCode.Space))
        {
            anim.SetTrigger(jumpHash);
        }
        if(Input.GetKeyDown(KeyCode.Keypad0))
        {
            anim.SetTrigger(deathHash);
        }
        if (Input.GetKeyUp(KeyCode.Space))
        {
            anim.SetTrigger(notjumpHash);
        }
        if (Input.GetKeyDown(KeyCode.A))
        {
            anim.SetTrigger(walkHash);
        }
        if(Input.GetKeyUp(KeyCode.A))
        {
            anim.SetTrigger(notwalkHash);
        }
    }
}
