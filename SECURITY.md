# Native Probe Security Boundary

These probes are measurements, not trust decisions.

- A clean summary does not prove absence of injection, namespace manipulation, or privileged mediation.
- `TracerPid` equal to zero does not prove that no hook or instrumentation exists.
- Overlay or tmpfs presence is not inherently malicious.
- Anonymous executable mappings may be legitimate, including runtime and JIT behavior.
- Paths are not returned by the initial public collectors to reduce fingerprinting and accidental disclosure.
- Input parsers are fixture-tested and must remain bounded.
- New probes must document target, producer, privileges, dependencies, races, false positives, false negatives, and privacy effects.

Do not add stealth behavior, destructive responses, process termination, private signatures, production bypass chains, or credential material.
