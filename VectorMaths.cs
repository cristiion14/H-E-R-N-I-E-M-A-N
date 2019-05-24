using System.Collections;
using System.Collections.Generic;
using UnityEngine;
public class VectorMaths : MonoBehaviour {

    //hypotenuse = magnitude of the vector (X2+Y2)
    //adiacent = X value of vector
    //opposite = Y value of vector

    // sin = y/hypo,  cos = x/hypo, tan = y/x       

   public class Matrix4by4
    {
        public static Matrix4by4 Identity
        {
            get
            {
                return new Matrix4by4(
                    new Vector4(1, 0, 0, 0),
                    new Vector4(0, 1, 0, 0),
                    new Vector4(0, 0, 1, 0),
                    new Vector4(0, 0, 0, 1));
            }
        }
        public Matrix4by4(Vector4 column1, Vector4 column2, Vector4 column3, Vector4 column4)
        {
            values = new float[4, 4];

            //Column1
            values[0, 0] = column1.x;
            values[1, 0] = column1.y;
            values[2, 0] = column1.z;
            values[3, 0] = column1.w;

            //Column2
            values[0, 1] = column2.x;
            values[1, 1] = column2.y;
            values[2, 1] = column2.z;
            values[3, 1] = column2.w;

            //Column3
            values[0, 2] = column3.x;
            values[1, 2] = column3.y;
            values[2, 2] = column3.z;
            values[3, 2] = column3.w;

            //Column4
            values[0, 3] = column4.x;
            values[1, 3] = column4.y;
            values[2, 3] = column4.z;
            values[3, 3] = column4.w;
        }
        public Matrix4by4(Vector3 column1, Vector3 column2, Vector3 column3, Vector3 column4)
        {
            values = new float[4, 4];

            //Column1
            values[0, 0] = column1.x;
            values[1, 0] = column1.y;
            values[2, 0] = column1.z;
            values[3, 0] = 0;

            //Column2
            values[0, 1] = column2.x;
            values[1, 1] = column2.y;
            values[2, 1] = column2.z;
            values[3, 1] = 0;

            //Column3
            values[0, 2] = column3.x;
            values[1, 2] = column3.y;
            values[2, 2] = column3.z;
            values[3, 2] = 0;

            //Column4
            values[0, 3] = column4.x;
            values[1, 3] = column4.y;
            values[2, 3] = column4.z;
            values[3, 3] = 1;
        }

        public float[,] values;

        public static Vector4 operator *(Matrix4by4 lhs, Vector4 vector)
        {
            vector.w = 1f;
            Vector4 result = new Vector4();
            result.x = lhs.values[0, 0] * vector.x + lhs.values[0, 1] * vector.y + lhs.values[0, 2] * vector.z + lhs.values[0, 3] * vector.w;
            result.y = lhs.values[1, 0] * vector.x + lhs.values[1, 1] * vector.y + lhs.values[1, 2] * vector.z + lhs.values[1, 3] * vector.w;
            result.z = lhs.values[2, 0] * vector.x + lhs.values[2, 1] * vector.y + lhs.values[2, 2] * vector.z + lhs.values[2, 3] * vector.w;
            result.w = lhs.values[3, 0] * vector.x + lhs.values[3, 1] * vector.y + lhs.values[3, 2] * vector.z + lhs.values[3, 3] * vector.w;

            return result;
        } 
        
        public static Matrix4by4 operator *(Matrix4by4 m1, Matrix4by4 m2)
        {
            Matrix4by4 result = new Matrix4by4(new Vector4(), new Vector4(), new Vector4(), new Vector4());
            result.values[0, 0] = m1.values[0, 0] * m2.values[0, 0] + m1.values[0, 1] * m2.values[1, 0] + m1.values[0, 2] * m2.values[2, 0] + m1.values[0, 3] * m2.values[3, 0];
            result.values[0, 1] = m1.values[0, 0] * m2.values[0, 1] + m1.values[0, 1] * m2.values[1, 1] + m1.values[0, 2] * m2.values[2, 1] + m1.values[0, 3] * m2.values[3, 1];
            result.values[0, 2] = m1.values[0, 0] * m2.values[0, 2] + m1.values[0, 1] * m2.values[1, 2] + m1.values[0, 2] * m2.values[2, 2] + m1.values[0, 3] * m2.values[3, 2];
            result.values[0, 3] = m1.values[0, 0] * m2.values[0, 3] + m1.values[0, 1] * m2.values[1, 3] + m1.values[0, 2] * m2.values[2, 3] + m1.values[0, 3] * m2.values[3, 3];

            result.values[1, 0] = m1.values[1, 0] * m2.values[0, 0] + m1.values[1, 1] * m2.values[1, 0] + m1.values[1, 2] * m2.values[2, 0] + m1.values[1, 3] * m2.values[3, 0];
            result.values[1, 1] = m1.values[1, 0] * m2.values[0, 1] + m1.values[1, 1] * m2.values[1, 1] + m1.values[1, 2] * m2.values[2, 1] + m1.values[1, 3] * m2.values[3, 1];
            result.values[1, 2] = m1.values[1, 0] * m2.values[0, 2] + m1.values[1, 1] * m2.values[1, 2] + m1.values[1, 2] * m2.values[2, 2] + m1.values[1, 3] * m2.values[3, 2];
            result.values[1, 3] = m1.values[1, 0] * m2.values[0, 3] + m1.values[1, 1] * m2.values[1, 3] + m1.values[1, 2] * m2.values[2, 3] + m1.values[1, 3] * m2.values[3, 3];

            result.values[2, 0] = m1.values[2, 0] * m2.values[0, 0] + m1.values[2, 1] * m2.values[1, 0] + m1.values[2, 2] * m2.values[2, 0] + m1.values[2, 3] * m2.values[3, 0];
            result.values[2, 1] = m1.values[2, 0] * m2.values[0, 1] + m1.values[2, 1] * m2.values[1, 1] + m1.values[2, 2] * m2.values[2, 1] + m1.values[2, 3] * m2.values[3, 1];
            result.values[2, 2] = m1.values[2, 0] * m2.values[0, 2] + m1.values[2, 1] * m2.values[1, 2] + m1.values[2, 2] * m2.values[2, 2] + m1.values[2, 3] * m2.values[3, 2];
            result.values[2, 3] = m1.values[2, 0] * m2.values[0, 3] + m1.values[2, 1] * m2.values[1, 3] + m1.values[2, 2] * m2.values[2, 3] + m1.values[2, 3] * m2.values[3, 3];

            result.values[3, 0] = m1.values[3, 0] * m2.values[0, 0] + m1.values[3, 1] * m2.values[1, 0] + m1.values[3, 2] * m2.values[2, 0] + m1.values[3, 3] * m2.values[3, 0];
            result.values[3, 1] = m1.values[3, 0] * m2.values[0, 1] + m1.values[3, 1] * m2.values[1, 1] + m1.values[3, 2] * m2.values[2, 1] + m1.values[3, 3] * m2.values[3, 1];
            result.values[3, 2] = m1.values[3, 0] * m2.values[0, 2] + m1.values[3, 1] * m2.values[1, 2] + m1.values[3, 2] * m2.values[2, 2] + m1.values[3, 3] * m2.values[3, 2];
            result.values[3, 3] = m1.values[3, 0] * m2.values[0, 3] + m1.values[3, 1] * m2.values[1, 3] + m1.values[3, 2] * m2.values[2, 3] + m1.values[3, 3] * m2.values[3, 3];
            return result;
        }
        
    };
    
    public class Quat
    {
        public float w, x, y, z;

        //Constructor that take in an axis angle
        public Quat(float angle, Vector3 axis)
        {
            float halfAngle = angle / 2;
            w = Mathf.Cos(halfAngle);
            x = axis.x * Mathf.Sin(halfAngle);
            y = axis.y * Mathf.Sin(halfAngle);
            z = axis.z * Mathf.Sin(halfAngle);
        }
        //Overloaded constr to store raw pos
        public Quat(Vector3 axis)
        {
            x = axis.x;
            y = axis.y;
            z = axis.z;
        }
        public Quat Inverse()
        {
            Quat rv = new Quat(new float(), new Vector3());
            rv.w = w;

            rv.SetAxis(-GetAxis());
            return rv;

        }

        public static Quat operator *(Quat lhs, Quat rhs)
        {
            //RS = (SwRw – Sv·Rv, SwRv + RwSv + Rv X Sv)
            //a.w* b.w - a.x * b.x - a.y * b.y - a.z * b.z,  // 1
            // a.w* b.x + a.x * b.w + a.y * b.z - a.z * b.y,  // i
            // a.w* b.y - a.x * b.z + a.y * b.w + a.z * b.x,  // j
            // a.w* b.z + a.x * b.y - a.y * b.x + a.z * b.w   // k
            //Cx = AyBz – AzBy
            //Cy = AzBx – AxBz
            //Cz = AxBy – AyBx

            Quat result = new Quat(new float(), new Vector3());

            result.x = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;  // 1
            result.y = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;  // i
            result.z = lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x;  // j
            result.w = lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w;   // k

            return result;

        }
        public void SetAxis(Vector3 q1)
        {

            this.x = q1.x;
            this.y = q1.y;
            this.z = q1.z;

        }
        public Vector3 GetAxis()
        {
           return new Vector3(this.x, this.y, this.z);
        }
        
        public Vector4 GetAxisAngle()
        {
            Vector4 rv = new Vector4();

            //Inverse cos to get half angle back
            float halfAngle = Mathf.Acos(w);
            rv.w = halfAngle * 2;   //full angle

            //Calculation to get our normal axis back using half angle
            rv.x = x / Mathf.Sin(halfAngle);
            rv.y = y / Mathf.Sin(halfAngle);
            rv.z = z / Mathf.Sin(halfAngle);

            return rv;
        }
        public static Quat SLERP(Quat q, Quat r, float t)
        {
            t = Mathf.Clamp(t, 0.0f, 1.0f);
            Quat d = r * q.Inverse();
            Vector4 AxisAngle = d.GetAxisAngle();
            Quat dT = new Quat(AxisAngle.w * t, new Vector3(AxisAngle.x, AxisAngle.y, AxisAngle.z));

            return dT * q;
        }
    }

    public class BoundingCircle
    {
        public Vector2 CentrePoint;
        public float radius;

        public BoundingCircle(Vector2 CentrePoint, float radius)
        {
            this.CentrePoint = CentrePoint;
            this.radius = radius;
        }

        public bool Intersects(BoundingCircle otherCircle)
        {
            Vector2 VectorToOther = otherCircle.CentrePoint - CentrePoint;

            float CombinedRadSQ = (otherCircle.radius + radius);
            CombinedRadSQ *= CombinedRadSQ;

            return VectorMaths.VectorLenSq(VectorToOther) <= CombinedRadSQ;
        }
    }

    public class AABB
    {
        public AABB(Vector3 Min, Vector3 Max)
        {
            minExtent = Min;
            maxExtent = Max;
        }
        public float Top
        {
            get { return maxExtent.y; }
        }

        public float Bottom
        {
            get { return minExtent.y; }
        }

        public float Left
        {
            get { return minExtent.x; }
        }
        public float Right
        {
            get { return maxExtent.x; }
        }
        public float Front
        {
            get { return maxExtent.z; }
        }
        public float Back
        {
            get { return minExtent.z; }
        }

        public static bool Intersects(AABB Box1, AABB Box2)
        {
            //box2.left > box1.right OR box2.right < box1.left OR box2.top < box1.bottom OR box2.bottom > box1.top
            return !(Box2.Left > Box1.Right
                || Box2.Right < Box1.Left
                || Box2.Top < Box1.Bottom
                || Box2.Bottom > Box1.Top);
        }

        Vector3 minExtent;
        Vector3 maxExtent;
    }
    public static Vector3 VectorAdd(Vector3 A, Vector3 B)
    {
        Vector3 result;
        result.x = A.x + B.x;
        result.y = A.y + B.y;
        result.z = A.z + B.z;
        return result;

    }

    public static Vector3 VectorSub(Vector3 A, Vector3 B)
    {
        Vector3 result;
        result.x = A.x - B.x;
        result.y = A.y - B.y;
        result.z = A.z - B.z;
        return result;
    }

    public static float VectorLen(Vector3 A)
    {
        return Mathf.Sqrt((A.x * A.x) + (A.y * A.y) + (A.z * A.z));
    }

    public static float  VectorLenSq(Vector3 A)
    {
        return ((A.x * A.x) + (A.y * A.y)+ (A.z * A.z));
    }

    public static Vector3 SacalarM(int x, Vector3 A)
    {
        Vector3 result;
        result.x = A.x * x;
        result.y = A.y * x;
        result.z = A.z * x;
        return result;
    }

    public static Vector3 VectorDiv(Vector3 A, int div)
    {
        Vector3 result;
        result.x = A.x / div;
        result.y = A.y / div;
        result.z = A.z / div;
        return result;
    }

    public static Vector3 VectorNormalized(Vector3 A)
    {
        Vector3 result;
        result.x = A.x / VectorLen(A);
        result.y = A.y / VectorLen(A);
        result.z = A.z / VectorLen(A);
        return result;
    }

    public static float DotProduct (Vector3 A, Vector3 B)
    {
        Vector3 NormA, NormB;
        float res;
        NormA = VectorNormalized(A);
        NormB = VectorNormalized(B);

        res = (NormA.x * NormB.x) + (NormA.y * NormB.y);
        return res;
    }

    //Workshop 3
    public static float VectorToRadian(Vector2 A)
    {
        float res = 0.0f;
        res = Mathf.Tan(A.y / A.x);

        return res;
    }

    public static Vector2 RadianToVector(float rad)
    {
        Vector2 angle = new Vector2();
        angle.x = Mathf.Cos(rad);
        angle.y = Mathf.Sin(rad);

        return angle;
    }

    public static Vector3 EulerAngletoDir(Vector3 eulerAngle)
    {
        // P(pitch)----X, R(Roll)-------Z, Yaw(Y)---------Y
        //Formula: Vx = cos(Y)cos(P)    Vy = sin(P)   Vz = cos(P) sin(Y)

        Vector3 result = new Vector3();
        result.x = Mathf.Cos(eulerAngle.y) * Mathf.Cos(eulerAngle.x);
        result.y = Mathf.Sin(eulerAngle.x);
        result.z = Mathf.Cos(eulerAngle.x) * Mathf.Sin(eulerAngle.y);

        return result;
    }

    public static Vector3 VectorCrossProduct(Vector3 A, Vector3 B)
    {
        /*A x B = C
          Cx = AyBz – AzBy
          Cy = AzBx – AxBz
          Cz = AxBy – AyBx 

        C is a vector that is perpendicular to A and B

        */

        Vector3 C = new Vector3();
        C.x = A.y * B.z - A.z * B.y;
        C.y = A.z * B.x - A.x * B.z;
        C.z = A.x * B.y - A.y * B.x;

        return C;

    }


    //Workshop 4
    public static Vector3 LinearInterpolateValue(Vector3 A, Vector3 B, float T)
    {
        //Def
        //Interpolation is a mathematical method of constructing new data points from known data points
        //Formula : C = A(1-T) + BT
        //A = known number (start)    B = known number(end) T = fractional value(usually between 0.0 and 1.0)  C = the interpolated value

        return A * (1.0f - T) + B * T;
    }

    public static Vector3 RotateVertexAroundAxis(float angle, Vector3 Axis, Vector3 Vertex)
    {
        //The Rodrigues rot formula
        //angle has to be rad
        Vector3 rv = (Vertex * Mathf.Cos(angle)) + DotProduct(Vertex, Axis) * Axis * (1 - Mathf.Cos(angle)) +
            VectorCrossProduct(Axis, Vertex) * Mathf.Sin(angle);

        return rv;
    }

}
