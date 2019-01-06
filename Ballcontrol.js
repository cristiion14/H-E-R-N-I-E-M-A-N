#pragma strict

var rotationSpeed = 100;
var jumpHeight = 8;
 var Hit01 : AudioClip;
 var Hit02 : AudioClip;
 var Hit03 : AudioClip;
 var distToGround : float;
 function Start() {
     distToGround = GetComponent.<Collider>().bounds.extents.y;// verifica cat de departe suntem de pamant
     if (Input.GetKeyDown(KeyCode.Space)|| Input.GetKeyDown(KeyCode.W) && IsGrounded())//daca esti pe pamant
     {
         GetComponent.<Rigidbody>().velocity.y = jumpHeight;
     }
 }
function Update () 
{
// handle rotation
var rotation : float = Input.GetAxis ("Horizontal") * rotationSpeed;
rotation *= Time.deltaTime;
GetComponent.<Rigidbody>().AddRelativeTorque (Vector3.back * rotation); 

 }
 function IsGrounded() : boolean   {
 return Physics.Raycast(transform.position, -Vector3.up, distToGround + 0.1);
 }

function OnCollisionEnter () {
 var theHit = Random.Range(0 , 3);
 if (theHit == 0 )
 {
 GetComponent.<AudioSource>().clip = Hit01;
 } 
 else if ( theHit == 1 )
 {
 GetComponent.<AudioSource>().clip = Hit02;
 }
 else {
 GetComponent.<AudioSource>().clip = Hit03;
 }
  GetComponent.<AudioSource>().pitch = Random.Range(0.9, 1.1);
  GetComponent.<AudioSource>().Play();
}