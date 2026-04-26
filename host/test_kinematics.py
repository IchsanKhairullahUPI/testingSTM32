"""
Pure-software validation of the kinematics module.
No hardware required. Run before touching any motors.
"""
import math
import robot_kinematics as rk


def rad_to_deg(v):
    return v * 180.0 / math.pi


def fk_pose_to_ik_input(pose):
    """
    forward_kinematics returns orientation in RADIANS (rotation vector form).
    SolveInverseKinematics expects orientation in DEGREES.
    This converts between them.
    """
    return list(pose[:3]) + [rad_to_deg(v) for v in pose[3:]]


def check_close(label, got, expected, tol=0.5):
    diff = abs(got - expected)
    status = "OK" if diff <= tol else "FAIL"
    print(f"  [{status}] {label}: got {got:.3f}, expected {expected:.3f}, diff {diff:.3f}")
    return diff <= tol


def test_home_pose():
    print("\n--- Test 1: forward kinematics at home position ---")
    print("Per the spreadsheet, joints [0,0,0,0,90,0] should give")
    print("X=286.83, Y=0, Z=433.77 (position in mm).")
    pose = rk.forward_kinematics([0.0, 0.0, 0.0, 0.0, 90.0, 0.0])
    print(f"  Got pose: {[round(v, 3) for v in pose]}")
    ok = True
    ok &= check_close("X", pose[0], 286.83)
    ok &= check_close("Y", pose[1], 0.0)
    ok &= check_close("Z", pose[2], 433.77)
    return ok


def test_roundtrip():
    # VER1: Exact joint match (may be too strict)
    #print("\n--- Test 2: FK -> IK round-trip ---")
    #print("If I compute the pose from some joints, then ask IK which")
    #print("joints produce that pose, I should get my original joints back.")
    #start = [10.0, 20.0, -15.0, 5.0, 45.0, 30.0]
    #pose = rk.forward_kinematics(start)
    #ik_input = fk_pose_to_ik_input(pose)
    #recovered = rk.SolveInverseKinematics(ik_input, start)
    #print(f"  Original:  {[round(j, 3) for j in start]}")
    #print(f"  Recovered: {[round(j, 3) for j in recovered]}")
    #ok = all(abs(a - b) < 0.1 for a, b in zip(start, recovered))
    #print(f"  [{'OK' if ok else 'FAIL'}] All joints within 0.1 degrees")
    #return ok

    # VER2: IK may return a different valid joint config reaching the same pose.
    print("\n--- Test 2: FK -> IK -> FK pose round-trip ---")
    print("IK may return a different valid joint config reaching the same pose.")
    print("So we check pose consistency, not joint consistency.")
    start = [10.0, 20.0, -15.0, 5.0, 45.0, 30.0]
    pose_a = rk.forward_kinematics(start)
    ik_input = fk_pose_to_ik_input(pose_a)
    recovered = rk.SolveInverseKinematics(ik_input, start)
    pose_b = rk.forward_kinematics(recovered)
    print(f"  Original joints:  {[round(j, 2) for j in start]}")
    print(f"  Recovered joints: {[round(j, 2) for j in recovered]}")
    print(f"  Original pose:    {[round(v, 3) for v in pose_a]}")
    print(f"  Recovered pose:   {[round(v, 3) for v in pose_b]}")
    position_ok = all(abs(a - b) < 0.5 for a, b in zip(pose_a[:3], pose_b[:3]))
    print(f"  [{'OK' if position_ok else 'FAIL'}] Position matches to within 0.5mm")
    return position_ok


def test_small_move():
    print("\n--- Test 3: small Cartesian move ---")
    print("Start at home, ask IK to move the tool +30mm in X.")
    print("Verify by running FK on the result and checking we got there.")
    current = [0.0, 0.0, 0.0, 0.0, 90.0, 0.0]
    home_pose = rk.forward_kinematics(current)
    target = fk_pose_to_ik_input(home_pose)
    target[0] += 30.0  # move 30mm in +X
    new_joints = rk.SolveInverseKinematics(target, current)
    print(f"  Joint deltas: {[round(n - c, 3) for n, c in zip(new_joints, current)]}")
    achieved = rk.forward_kinematics(new_joints)
    ok = True
    ok &= check_close("X reached", achieved[0], target[0], tol=0.5)
    ok &= check_close("Y unchanged", achieved[1], target[1], tol=0.5)
    ok &= check_close("Z unchanged", achieved[2], target[2], tol=0.5)
    return ok


def test_raw_roundtrip():
    print("\n--- Test 4: raw FK -> raw IK (bypass SolveInverseKinematics) ---")
    print("This calls inverse_kinematics directly, bypassing degree conversion")
    print("and the multi-solution search. If THIS passes but test 2 fails,")
    print("the bug is in SolveInverseKinematics or its Python wrapper,")
    print("not the core C++ math.")
    start = [10.0, 20.0, -15.0, 5.0, 45.0, 30.0]
    pose_a = rk.forward_kinematics(start)
    # NOTE: we pass the pose unchanged — no radian->degree conversion
    recovered = rk.inverse_kinematics(list(pose_a), start)
    pose_b = rk.forward_kinematics(recovered)
    print(f"  Original joints:  {[round(j, 2) for j in start]}")
    print(f"  Recovered joints: {[round(j, 2) for j in recovered]}")
    print(f"  Position error:   dx={pose_b[0]-pose_a[0]:.3f} dy={pose_b[1]-pose_a[1]:.3f} dz={pose_b[2]-pose_a[2]:.3f}")
    position_ok = all(abs(a - b) < 0.5 for a, b in zip(pose_a[:3], pose_b[:3]))
    print(f"  [{'OK' if position_ok else 'FAIL'}] Position matches to within 0.5mm")
    return position_ok


def test_zero_rotation():
    print("\n--- Test 5: FK -> IK with identity orientation ---")
    print("Tests position handling in isolation. If rotation is zero,")
    print("the orientation-format bug (if any) doesn't trigger.")
    start = [10.0, 20.0, -15.0, 0.0, 0.0, 0.0]
    pose_a = rk.forward_kinematics(start)
    print(f"  Pose from start joints: {[round(v, 3) for v in pose_a]}")
    # Try with zero rotation target
    zero_rot_target = list(pose_a[:3]) + [0.0, 0.0, 0.0]
    recovered = rk.inverse_kinematics(zero_rot_target, start)
    pose_b = rk.forward_kinematics(recovered)
    print(f"  Recovered joints: {[round(j, 2) for j in recovered]}")
    print(f"  Position error: dx={pose_b[0]-pose_a[0]:.3f} dy={pose_b[1]-pose_a[1]:.3f} dz={pose_b[2]-pose_a[2]:.3f}")


if __name__ == "__main__":
    # TEST 1
    rk.robot_set()
    print("DH parameters loaded.")
    results = [
        ("home pose", test_home_pose()),
        ("round-trip", test_roundtrip()),
        ("small move", test_small_move()),
    ]
    print("\n=== Summary ===")
    for name, ok in results:
        print(f"  {name}: {'PASS' if ok else 'FAIL'}")

    # TEST 2
    candidates = {
        "as-is":       [0, 0, 0, 0, 90, 0],
        "J5+90":       [0, 0, 0, 0, 180, 0],
        "J3+180":      [0, 0, 180, 0, 90, 0],
        "J3+180,J5+90":[0, 0, 180, 0, 180, 0],
        "all zero":    [0, 0, 0, 0, 0, 0],
    }
    for name, joints in candidates.items():
        pose = rk.forward_kinematics(joints)
        print(f"{name:20s} -> X={pose[0]:7.2f} Y={pose[1]:7.2f} Z={pose[2]:7.2f}")

    # TEST 3
    test_raw_roundtrip()
    test_zero_rotation()