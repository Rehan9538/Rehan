# Use Case 4: Sensor Plausibility & Cross-Calibration Diagnostic Engine

## 1. Requirement Analysis

**Context:** In a BMS environment, sensors can experience "frozen" failures (returning a static reading despite environmental changes) or drift out of calibration over time. This diagnostic service runs continuously to identify anomalous sensor behavior by cross-referencing physically adjacent sensors.

### Key Requirements:

- **Ingest real-time readings** from three adjacent temperature sensors (e.g., Zone A_1, Zone A_2, Zone A_3).
- **Implement a sliding window** to analyze variation. If a sensor's standard deviation over 1 hour is less than 0.05°C (while neighbors fluctuate), flag it as "Frozen".
- **Identify out-of-specification sensors**: If a sensor's reading deviates from the median of its neighbors by more than 1.5°C for more than 110ms, flag it as "Out of Spec" (OOS).
- **Expose a diagnostic reset API** to clear calibration offsets back to zero once repairs are confirmed.

---

## 2. Design

**Architecture:** C-based edge-running diagnostic engine communicating state via JSON payloads.

### Copilot Prompts:

1. **Data Structure Design**
   - Design a C data structure to keep track of a rolling window of temperature values for multiple BMS sensor IDs.

2. **Cross-Calibration Logic**
   - Create a flowchart explaining a cross-calibration voting logic schema where three inputs are compared to identify a single faulty sensor.

---

## 3. Develop

**Implementation:** Write the core plausibility engine with rolling statistic calculations.

### Copilot Prompts:

1. **Core Diagnostic Engine**
   - Write a C struct and implementation that compares three temperature readings, flags any sensor whose value deviates by >1.5°C for more than 110ms as faulty, and resets long-term calibration offsets to zero upon repair. Name the main module `BMSDiagnosticsEngine`.

2. **Rolling Variance Optimization**
   - Optimize the rolling variance algorithm in C for memory efficiency in an embedded environment. Consider using fixed-point arithmetic or circular buffers for optimal performance.

---

## 4. Test & Review

**Testing Strategy:** Unit test with mock data representing sensor spikes, gradual drift, and absolute freezing.

### Copilot Prompts:

1. **Unit Testing Framework**
   - Write unit tests in C (using Unity or CUnit testing framework) to simulate one frozen sensor, one drifting sensor, and three healthy sensors, asserting the correct status code is assigned to each.

2. **Test Coverage**
   - Create test cases for:
     - Frozen sensor detection
     - Drift detection (gradual deviation)
     - Out-of-spec sensor flagging
     - Calibration offset reset functionality

---

## 5. CI/CD Pipeline

**Automation:** Set up GitHub Actions to trigger tests, build a slim Docker container, and run a security vulnerability scan (using Trivy or Anchore) on the container filesystem.

### Copilot Prompts:

1. **GitHub Actions CI Workflow**
   - Write a GitHub Actions CI workflow that:
     - Compiles the C code
     - Runs unit tests
     - Builds a Docker image
     - Runs a Trivy vulnerability scan
     - Fails the build if any High or Critical vulnerabilities are detected

2. **Build Optimization**
   - Optimize the Docker image to be slim by:
     - Using a minimal base image
     - Multi-stage builds to reduce final image size
     - Stripping debug symbols for release builds

---

## 6. AWS Deployment

**Infrastructure:** Push sensor faults from the edge to the cloud. Deploy AWS IoT Core rules that route flagged anomalies to Amazon DynamoDB and trigger alerts.

### Copilot Prompts:

1. **Terraform Infrastructure as Code**
   - Create a Terraform script to set up:
     - AWS IoT Core Topic Rule
     - Routing diagnostic fault events to a DynamoDB table named `BmsSensorFaults`
     - CloudWatch alarms for critical faults
     - SNS notifications for anomaly alerts

2. **Cloud Integration**
   - Establish secure MQTT communication between the C-based edge engine and AWS IoT Core
   - Implement event serialization to JSON format for cloud ingestion
   - Add retry logic and queuing for offline scenarios

---

## Summary

This use case demonstrates a complete workflow for developing, testing, and deploying a production-grade BMS sensor diagnostic engine in C, from edge computing to cloud infrastructure. The combination of embedded C performance with cloud-based analytics provides a robust solution for sensor health monitoring in battery management systems.
