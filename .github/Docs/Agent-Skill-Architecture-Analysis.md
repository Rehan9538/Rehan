# Agent vs. Skill Architecture Analysis
**Date:** 2026-09-29  
**Project:** BMS Sensor Plausibility Diagnostic Engine  
**Analysis:** Should we split Agent.md into Agent + Skill?

---

## Current State: Monolithic Agent.md (486 lines)

**Problem:** Single file mixing two distinct concerns:
1. **Definitional/Schema** (sections 1-10): Test design rules, templates, schema validation
2. **Execution/Procedural** (sections 11-13): Test running, validation, gate enforcement

This coupling creates maintenance friction:
- Test designers need schema rules (passive knowledge)
- CI/CD pipeline needs execution procedures (active work)
- Both live in same file → conflicts in workflow

---

## Recommended Architecture: Agent + Skill Split

### Option: Create 1 Skill + 1 Agent (RECOMMENDED)

#### **SKILL: UT Test Design & Definition**
**Location:** `.github/Skills/UT-test-design-rules/SKILL.md` (or similar)

**Purpose:** Passive, reusable definition of what good tests look like

**Content:**
- Test schema rules (18 required fields)
- Test ID format rules
- Status value enums
- Category definitions
- CUnit assertion rules
- Configuration parameter rules
- Test template (template structure)
- Examples of well-formed test cases

**Who Uses It:**
- ✅ Test designers writing new tests
- ✅ Code reviewers validating test PRs
- ✅ Linters/validators checking test format
- ✅ Documentation generators
- ✅ Agents (as input/reference)

**Benefits:**
- Reusable across multiple agents
- Can be referenced in test templates
- Becomes single source of truth for "what is a valid test?"
- Useful independently of CI/CD

---

#### **AGENT: UT Test Execution & Validation**
**Location:** `.github/Agents/UT-test-execution/Agent.md` (existing location)

**Purpose:** Active test execution, validation, result processing

**Content:**
- Execution contract (references skill)
- Quality gates (references skill)
- 10 test validation procedures (sections 11.1-11.10)
- 6-phase validation algorithm (section 12)
- Acceptance criteria checklist (section 13)

**Workflow:**
1. Load test cases (from prompt or test files)
2. Validate against skill rules (schema, format, CUnit)
3. Execute: cmake build → ctest run → lcov coverage
4. Process results: update status, generate defects
5. Generate reports: CSV files, summaries
6. Enforce quality gates (skill + agent rules)

**Who Uses It:**
- ✅ CI/CD pipeline (automated test execution)
- ✅ Test validation workflow
- ✅ Release readiness verification
- ✅ Coverage enforcement

**Benefits:**
- Focused on action/execution
- Uses skill as reference, not repetition
- Clear procedures for each validation phase
- Deterministic gates with no ambiguity

---

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│  .github/Prompts/UT-test-design_prompt.md (AUTHORITATIVE)      │
│  - Master specification for all UT requirements                │
└────────────┬──────────────────────────────────────────┬─────────┘
             │                                          │
             ↓                                          ↓
┌──────────────────────────────────┐    ┌──────────────────────────────────┐
│ SKILL: UT-test-design-rules      │    │ AGENT: UT-test-execution         │
│ .github/Skills/SKILL.md          │    │ .github/Agents/Agent.md          │
├──────────────────────────────────┤    ├──────────────────────────────────┤
│ Schema & Definition Rules:       │    │ Execution & Validation:          │
│ ✓ Test ID format                 │    │ ✓ Test execution workflow        │
│ ✓ Status values                  │    │ ✓ CUnit assertion validation     │
│ ✓ Required fields (18)           │    │ ✓ Code coverage enforcement      │
│ ✓ Categories (7 types)           │    │ ✓ P0 test verification           │
│ ✓ CUnit assertion patterns       │    │ ✓ Defect linkage & root cause    │
│ ✓ Test template structure        │    │ ✓ Test data validation           │
│ ✓ Configuration rules            │    │ ✓ Traceability matrix generation │
│ ✓ Baseline test set (16 tests)   │    │ ✓ Status table updates           │
│ ✓ Examples of good tests         │    │ ✓ CSV output validation          │
│ ✓ Validation rules               │    │ ✓ Execution time monitoring      │
│                                  │    │                                  │
│ Used by: Test designers, linters,│    │ Used by: CI/CD, test automation, │
│ reviewers, documentation         │    │ release verification             │
└──────────────────────────────────┘    └──────────────────────────────────┘
             ↑                                          ↑
             │                                          │
             └──────────────────┬──────────────────────┘
                                │
                    References skill for validation rules
                    Enforces skill schema in agent gates
```

---

## Detailed Split: What Goes Where

### SKILL Content (~200 lines)
```markdown
# Unit Test (UT) Design & Definition Skill

## 1. Role & Purpose
- Define what constitutes a valid test case
- Provide reusable validation rules
- Serve as reference for test designers

## 2. Authoritative Input
- Reference to UT-test-design_prompt.md
- Reference to Agent.md for execution procedures

## 3. Test Schema Rules
- Test ID format: MODULE_UT_SEQUENCE_VARIANT
- Status values: PASS, FAIL, NOT_APPLICABLE, NOT_STARTED, BLOCKED
- 18 required template fields

## 4. Test Categories
- Freeze Detection, Drift Detection, Outlier Filter
- State Machine, Edge Cases, Configuration, MQTT/Reporting

## 5. CUnit Assertion Rules
- Valid macros: CU_ASSERT_*, CU_ASSERT_EQUAL, etc.
- Pattern matching for assertion validation
- Examples of good assertions

## 6. Test Template
Complete test case template with all 18 fields

## 7. Baseline Test Set
16 required tests with categories and priorities

## 8. Validation Rules
Rules for validating tests conform to schema

## 9. Example: Well-Formed Test Case
Complete working example

## 10. Quick Reference Checklist
- 18 fields checklist
- 7 categories checklist
- CUnit assertion rules checklist
```

### AGENT Content (~286 lines)
```markdown
# Unit Test (UT) Execution & Validation Agent

## 1. Role & Execution Contract
- Execute tests according to skill schema
- Validate results against skill rules
- Generate reports and enforce gates

## 2. Quality Gates (Using Skill)
- Gate 1: Prompt Compliance (validate against skill)
- Gate 2: Coverage (validate using skill metrics)
- Gate 3: Execution & Metrics (skill thresholds)
- Gate 4: Data Integrity (skill rules)

## 3-13. Test Validation Procedures
- 11. Test Validation Execution Procedures (10 procedures)
- 12. Complete Test Validation Algorithm (6 phases)
- 13. Acceptance Criteria Compliance Checklist

## 14. Integration with Skill
- How agent uses skill for validation
- How to report skill violations
```

---

## Benefits of This Split

### 1. Separation of Concerns ✅
**Skill:** "What is a valid test?"  
**Agent:** "How do I run and validate tests?"

These are fundamentally different questions. Skill answers design question. Agent answers execution question.

### 2. Reusability ✅
**Skill can be used by:**
- Test generators (autogenerate tests matching schema)
- Linters (validate test format without running)
- Documentation (reference test template)
- Multiple agents (different validation workflows)
- Test review checklists (manual review against skill)

**Agent is specific to:**
- Test execution automation
- Release verification gate
- CI/CD pipeline integration

### 3. Maintainability ✅
**Updating test schema:**
- Change skill only
- All agents automatically get updates
- No agent-specific schema duplication

**Updating test procedures:**
- Change agent only
- Skill remains stable
- Other users of skill unaffected

### 4. Clarity & Navigation ✅
**Test designer asks:** "What must my test include?"  
→ Goes to SKILL section 6 (Test Template)

**Test engineer asks:** "How do I execute and validate tests?"  
→ Goes to AGENT section 11-13 (Procedures & Algorithm)

**CI/CD asks:** "What are my validation gates?"  
→ Goes to AGENT section 2 (Quality Gates)

### 5. Team Scalability ✅
**As team grows:**
- Test writers: "Read the Skill"
- Test automation: "Read the Agent"
- Release manager: "Read Agent gates section"
- Documentation: "Reference Skill template"

No single file is a bottleneck.

### 6. Composition & Extension ✅
**Example: Create specialized agents**
```
UT-test-execution/Agent.md (current + skill reference)
  Uses: UT-test-design-rules/SKILL.md

Could create later:
UT-test-linter/Agent.md (validates test format)
  Uses: UT-test-design-rules/SKILL.md
  Uses: ESLint/similar tool

UT-test-generator/Agent.md (creates test stubs)
  Uses: UT-test-design-rules/SKILL.md
  Template: from skill
```

### 7. Documentation & Onboarding ✅
**New team member:**
- "Here's the skill: 5-min read on what tests need"
- "Here's the agent: detailed procedures for running tests"
- Faster onboarding, clearer roles

---

## Potential Drawbacks

### 1. File Multiplication
**Before:** 1 Agent file (486 lines)  
**After:** 2 files (Skill ~200 + Agent ~286)

**Mitigation:** Both files live in repo, clear naming convention

### 2. Cross-File References
**Need to maintain:** Skill → Agent → Prompt links

**Mitigation:** Use explicit "References" sections at top of each file

### 3. Slight Learning Curve
**Need to explain:** When to use skill vs. agent

**Mitigation:** Create diagram (like above) in README

---

## Recommendation: YES, Split into Agent + Skill

### Why it's Beneficial for This Project:

1. **Immediate Value:**
   - Test designers get clear 5-minute reference (skill)
   - CI/CD pipeline gets procedural guide (agent)
   - No confusion about which is which

2. **Future Growth:**
   - Can add test linter (uses skill)
   - Can add test generator (uses skill)
   - Can add alternative validators (use skill)
   - All reference same skill = consistency

3. **Maintenance:**
   - Change test schema once (skill)
   - All users auto-aligned
   - No repeated schema rules

4. **Clarity:**
   - 200 lines of "what tests must look like" (skill)
   - 286 lines of "how to run tests" (agent)
   - Each file has single purpose

---

## Implementation Plan

### Step 1: Create SKILL File
**Location:** `.github/Skills/UT-test-design-rules/SKILL.md`  
**Content:** Sections 1-10 from current Agent.md  
**Lines:** ~200

### Step 2: Refactor AGENT File
**Location:** `.github/Agents/Agent.md` (update in place)  
**Content:** 
- Authoritative Input → References skill + prompt
- Execution Contract → "Read skill first, then proceed"
- Sections 11-13 → Keep execution procedures
- Add section 14 → "How Agent Uses Skill"

**Lines:** ~286

### Step 3: Create Integration Document
**Location:** `.github/Docs/Agent-Skill-Architecture.md`  
**Content:** Diagrams, relationships, usage guide

### Step 4: Update References
- Prompt file → Add reference to skill
- README → Explain skill vs. agent
- Test templates → Reference skill directly

---

## Files to Create

1. **`.github/Skills/UT-test-design-rules/SKILL.md`** (~200 lines)
   - Test schema, rules, template, examples

2. **`.github/Agents/Agent.md`** (refactored, ~286 lines)
   - Execution procedures, algorithm, gates
   - References skill instead of repeating it

3. **`.github/Docs/Agent-Skill-Architecture.md`** (~50 lines)
   - Architecture diagram
   - Usage guide
   - Integration examples

---

## Answer to Your Question: Is It Beneficial?

**YES, absolutely beneficial for this project because:**

✅ Clear separation: Design (skill) vs. Execution (agent)  
✅ Reusability: Skill used by multiple agents/tools  
✅ Maintainability: Update schema in one place  
✅ Scalability: Team can grow without file conflicts  
✅ Clarity: Test designer and CI engineer have separate docs  
✅ Composability: Can build specialized agents using skill  
✅ Onboarding: New members have clear entry points  

**Recommended approach:** 1 Skill + 1 Agent (not 2 agents)
- Skill = Passive definition (test rules)
- Agent = Active execution (validation procedures)

This creates a clean, maintainable, scalable architecture for BMS UT management.
