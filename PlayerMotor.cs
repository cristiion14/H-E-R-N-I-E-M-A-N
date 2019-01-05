using UnityEngine;

[RequireComponent(typeof(Rigidbody))]

public class PlayerMotor : MonoBehaviour {
    [SerializeField]
    private Camera cam;
    
    
    private Vector3 velocity = Vector3.zero;
    private Vector3 rotation = Vector3.zero;
    private float cameraRotation = 0f;
   
    private float currentCameraRotx = 0f;
    private Rigidbody rb;
    [SerializeField]
    private float cameraRotationLimit = 85f;
    void Start()
    {
        rb = GetComponent<Rigidbody>();

    }
    //gets a movement vector
    
    public void Move(Vector3 _velocity)
    {
        velocity = _velocity;
    }
    //gets a rotational vector

    public void Rotate(Vector3 _rotation)
    {
        rotation = _rotation;
    }

    //gets a rotational vector for the camera
    public void RotateCamera(float _cameraRotation)
    {
        cameraRotation = _cameraRotation;
    }
    
    //run every physics iteration

    void FixedUpdate()
    {
        PerformMovement();
        PerformRotation();
    }
    //Perform movement based on velocity variable
    void PerformMovement()
    {
        if(velocity!=Vector3.zero)
        {
            rb.MovePosition(rb.position + velocity * Time.fixedDeltaTime);
        }
    }
    //perform rotation
    void PerformRotation()
    {
        rb.MoveRotation(rb.rotation * Quaternion.Euler(rotation));
        if(cam != null)
        {
            //set our rotation and clamp it
            currentCameraRotx -= cameraRotation;
            currentCameraRotx = Mathf.Clamp(currentCameraRotx, -cameraRotationLimit, cameraRotationLimit);
            
            //apply the rotation to the transform of the camera.
            cam.transform.localEulerAngles = new Vector3(currentCameraRotx, 0f, 0f);
        }
    }
}
