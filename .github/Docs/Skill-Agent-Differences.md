# Skill vs Agent: Architecture and Differences
## Unit Test Design and Execution Framework

**Date:** 2026-09-29  
**Document:** Comprehensive guide to Skill and Agent separation  
**Audience:** Test designers, CI/CD engineers, reviewers, developers

---

## Quick Summary

| Aspect | SKILL | AGENT |
|--------|-------|-------|
| **What is it?** | **Passive definition** of test schema | **Active execution** of test validation |
| **Answers** | "What should a test look like?" | "How do I run and validate tests?" |
| **User Type** | Test writers, reviewers, linters | CI/CD pipeline, automation |
| **Updates** | When test rules change | When validation procedures change |
| **Reusability** | High (used by multiple tools) | Specific to execution workflow |
| **Size** | ~200 lines (rules + examples) | ~286 lines (procedures + gates) |
| **Location** | `.github/Skills/UT-test-design-rules/SKILL.md` | `.github/Agents/Agent.md` |

---

## 1. What is a SKILL?

### Definition
A **Skill** is a **reusable, passive body of knowledge** about a specific domain. It answers: "What does this domain look like?"

### UT Test Design Skill

**Purpose:** Define what constitutes a valid, well-formed test case for the BMS diagnostic engine.

**Content (200 lines):**
- Test schema rules
- Test ID format requirements
- Status value enums
- Required template fields (18)
- Test categories (7 types)
- CUnit assertion rules
- Configuration parameter rules
- Baseline test set (16 tests)
- Examples of valid/invalid tests
- Validation checklist

**Who Uses It:**
- ✅ **Test Designers:** "Here's what my test must include"
- ✅ **Code Reviewers:** "Does this PR's test match the Skill?"
- ✅ **Linters:** "This test violates Skill rule X"
- ✅ **Documentation Generators:** "Use Skill template to generate docs"
- ✅ **Test Generators:** "Create tests matching Skill schema"
- ✅ **Agents:** "Validate tests against Skill"

### Skill Characteristics
- **Read-Only Usage:** Users read from Skill, don't modify tests based on Skill
- **Reference Function:** Answers "what should this look like?" question
- **Multiple Consumers:** Can be used by many different tools/processes
- **Stable:** Changes only when rules/schema change
- **Reusable:** Not specific to any single workflow

---

## 2. What is an AGENT?

### Definition
An **Agent** is an **active, procedural workflow** that performs actions. It answers: "How do I execute and validate?"

### UT Test Execution & Validation Agent

**Purpose:** Execute unit tests, validate results, enforce quality gates, and generate reports.

**Content (286 lines):**
- Execution procedures (sections 11.1-11.10)
- 6-phase validation algorithm
- Quality gates (4 gates enforced)
- Acceptance criteria checklist
- Integration with Skill

**Procedures Included:**
1. Test Execution Workflow (cmake, ctest, parsing)
2. CUnit Assertion Validation
3. Code Coverage Validation (lcov integration)
4. P0-Critical Test Pass Verification
5. Defect Linkage & Root Cause Analysis
6. Test Data Validation
7. Traceability Matrix Generation
8. Status Table Updates
9. CSV Output Validation
10. Execution Time Monitoring

**Who Uses It:**
- ✅ **CI/CD Pipeline:** "Execute tests and report results"
- ✅ **Test Automation:** "Run tests and generate coverage reports"
- ✅ **Release Manager:** "Verify all quality gates pass"
- ✅ **Test Engineers:** "Understand validation procedures"

### Agent Characteristics
- **Action-Oriented:** Performs tasks, not just defines rules
- **Procedural:** Step-by-step workflows and algorithms
- **Workflow-Specific:** Tailored to test execution pipeline
- **Deterministic:** Clear gate enforcement with pass/fail logic
- **Reports:** Generates output artifacts (CSV, summaries)

---

## 3. Key Differences

### 3.1 Purpose

**SKILL:**
- Answers: "What is valid?"
- Defines: Standards, rules, templates
- Example question: "What are the 18 required fields in a test case?"
- Answer location: Skill section 2

**AGENT:**
- Answers: "How do I execute?"
- Defines: Procedures, workflows, gates
- Example question: "What steps validate that a test is PASS?"
- Answer location: Agent section 11.1-11.10

### 3.2 Content Comparison

```
┌─────────────────────────────────────────────────────────────┐
│ SKILL (Passive Definition)                                  │
├─────────────────────────────────────────────────────────────┤
│ Section 1: Schema Rules                                     │
│ - Test ID format: MODULE_UT_SEQUENCE_VARIANT               │
│ - Status values: PASS, FAIL, NOT_APPLICABLE, etc.          │
│ - Categories: 7 types (Freeze, Drift, etc.)                │
│                                                              │
│ Section 2: Required Fields (18)                            │
│ - Test ID, Test Name, Category, Priority, etc.             │
│                                                              │
│ Section 3: CUnit Rules                                      │
│ - Allowed macros: CU_ASSERT_EQUAL, CU_ASSERT_DOUBLE, etc.  │
│                                                              │
│ Section 7: Test Template                                    │
│ - Complete example test case with all 18 fields filled     │
│                                                              │
│ Section 8: Validation Checklist                            │
│ - 20+ checkpoints to verify test conformance               │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ AGENT (Active Execution)                                    │
├─────────────────────────────────────────────────────────────┤
│ Section 11: Test Validation Procedures                     │
│ - 11.1 Test Execution Workflow (cmake, ctest)              │
│ - 11.2 CUnit Assertion Validation (parse & validate)       │
│ - 11.3 Code Coverage Validation (lcov, 80% gate)           │
│ - 11.4 P0 Test Verification (release readiness gate)       │
│ - 11.5 Defect Linkage (DEFECT ID generation)               │
│ - 11.6 Test Data Validation                                │
│ - 11.7 Traceability Matrix (SWDD↔Test mapping)             │
│ - 11.8 Status Table Update                                 │
│ - 11.9 CSV Output Validation (RFC 4180)                    │
│ - 11.10 Execution Time Monitoring (<30s gate)              │
│                                                              │
│ Section 12: Complete Algorithm (6 phases)                  │
│ - Phase 1: Pre-Execution Validation                        │
│ - Phase 2: Build & Execution                               │
│ - Phase 3: Coverage Analysis                               │
│ - Phase 4: Result Processing & Defects                     │
│ - Phase 5: Report Generation                               │
│ - Phase 6: Final Compliance Gate                           │
│                                                              │
│ Section 13: Acceptance Criteria Checklist                  │
│ - 8 criteria from UT prompt, each with sub-checks          │
└─────────────────────────────────────────────────────────────┘
```

### 3.3 Workflow Positioning

```
Test Designer's Workflow:
┌──────────────────────────────────────────┐
│ 1. Read Skill (what must I include?)     │ ← Skill used here
│    ├─ Section 1: ID format               │
│    ├─ Section 2: 18 required fields      │
│    ├─ Section 7: Template                │
│    └─ Section 8: Validation checklist    │
│                                          │
│ 2. Write test case using template       │ ← Skill is reference
│                                          │
│ 3. Run Agent validation (PR review)      │ ← Agent used here
└──────────────────────────────────────────┘

CI/CD Pipeline Workflow:
┌──────────────────────────────────────────┐
│ 1. Load tests from repo                  │
│                                          │
│ 2. Agent Phase 1: Validate vs Skill      │ ← Skill as reference
│    ├─ Check ID format (Skill 1.1)        │
│    ├─ Check 18 fields (Skill 2)          │
│    └─ Check CUnit syntax (Skill 3)       │
│                                          │
│ 3. Agent Phase 2-3: Build & run tests    │ ← Agent executes
│                                          │
│ 4. Agent Phase 4-5: Process & report     │ ← Agent generates output
│                                          │
│ 5. Agent Phase 6: Enforce gates          │ ← Agent blocks on failure
└──────────────────────────────────────────┘
```

### 3.4 User Questions

**Test Designer (Uses Skill):**
- "What does my test need to include?" → Skill section 2 (18 fields)
- "What is valid test ID format?" → Skill section 1.1
- "How do I write CUnit assertions?" → Skill section 3
- "Here's a template, fill it in" → Skill section 7
- "How do I validate my test?" → Skill section 8 checklist

**CI/CD Engineer (Uses Agent):**
- "How do I run all tests?" → Agent section 11.1
- "How do I check code coverage?" → Agent section 11.3
- "What blocks a release?" → Agent section 11.4 (P0 gate)
- "How do I track defects?" → Agent section 11.5
- "What reports do I generate?" → Agent section 11.9
- "What's the complete workflow?" → Agent section 12 (6 phases)

---

## 4. Reusability

### Skill Reusability (High)

The Skill can be used by multiple tools and processes:

```
.github/Skills/UT-test-design-rules/SKILL.md
├─ Test Designer
│  ├─ Reading test requirements
│  └─ Validating against template
│
├─ Code Reviewer
│  ├─ Checking PR test format
│  ├─ Validating 18 fields present
│  └─ Using section 8 checklist
│
├─ Test Linter Tool (future)
│  ├─ Parsing test files
│  ├─ Validating against Skill rules
│  └─ Reporting violations
│
├─ Test Generator (future)
│  ├─ Creating test stubs
│  ├─ Using Skill template
│  └─ Pre-populating required fields
│
├─ Documentation Generator (future)
│  ├─ Extracting test info
│  ├─ Formatting against Skill template
│  └─ Creating test reference docs
│
└─ Agent (Test Execution)
   ├─ Validating test structure (Skill 1, 2, 3)
   ├─ Enforcing baseline test set (Skill 6)
   └─ Checking output format (Skill output rules)
```

### Agent Reusability (Low)

The Agent is specific to the test execution workflow. Other agents could be created:

```
Specialized agents (future) using same Skill:

UT-test-execution/Agent.md (current)
├─ Uses: Skill for test validation
├─ Purpose: Run tests, generate reports
└─ Workflow: 6-phase execution

UT-test-linter/Agent.md (potential)
├─ Uses: Skill for format validation
├─ Purpose: Check test compliance
└─ Workflow: Syntax checking, rules enforcement

UT-test-generator/Agent.md (potential)
├─ Uses: Skill for template reference
├─ Purpose: Create test stubs
└─ Workflow: Template → stub generation

All agents reference SAME Skill = Consistency
```

---

## 5. Maintenance & Updates

### When Skill Changes

**Scenario:** Test schema rules change (e.g., add new category, change status values)

**What to update:**
- Update Skill file ONLY
- All consumers automatically aligned:
  - Test designers write to new schema
  - Reviewers validate against new schema
  - Agent validates against new schema
  - Future tools use updated schema

**Result:** Single point of change, no duplication

### When Agent Changes

**Scenario:** Execution procedures change (e.g., new validation step, different gate logic)

**What to update:**
- Update Agent file ONLY
- Skill remains unchanged
- Other users of Skill unaffected
- Next test run uses new procedures

**Result:** Procedural changes don't affect schema definitions

---

## 6. Integration: How Skill & Agent Work Together

### Step-by-Step Integration

```
Step 1: Test Designer Creates Test
  └─ Reads Skill sections 1, 2, 7, 8
  └─ Uses template from Skill section 7
  └─ Validates against Skill checklist (section 8)
  └─ Submits PR with test case

Step 2: Code Reviewer Reviews Test PR
  └─ Reads Skill sections 1, 2, 3, 8
  └─ Uses Skill checklist (section 8) to review
  └─ Approves if test matches Skill schema
  └─ Merges test to repository

Step 3: CI/CD Pipeline Runs (Agent)
  └─ Agent loads test from repo
  └─ Agent loads Skill from repo
  └─ Agent Phase 1 (Pre-execution):
     └─ Validates test structure against Skill sections 1, 2, 3
     └─ GATE: Blocks if any Skill rule violated
  └─ Agent Phase 2-3 (Build & coverage):
     └─ Executes test using procedures in Agent section 11
     └─ GATE: Blocks if coverage <80% or P0 test FAIL
  └─ Agent Phase 5 (Report generation):
     └─ Generates CSV using Skill field names
     └─ CSV validated against Skill output rules
  └─ Agent Phase 6 (Final gate):
     └─ Verifies all Skill schema satisfied
     └─ Blocks or approves release

Result: Skill enforced at design time (review) and runtime (execution)
```

### Skill Sections Used by Agent

| Skill Section | Agent Uses For | Agent Section |
|---------------|----------------|---------------|
| 1.1 Test ID format | Validate test IDs | 11.1 |
| 1.2 Status values | Update status correctly | 11.4, 11.8 |
| 1.3 Categories | Validate category assignment | 11.6 |
| 2 Required fields (18) | Pre-execution validation | 11.1 |
| 3 CUnit assertions | Parse assertions | 11.2 |
| 4-5 Config & data | Validate mock data | 11.6 |
| 6 Baseline test set | Enforce 16 tests + 7 categories | Gate 2 |
| 8 Validation checklist | Reference for quality checks | Gate 1 |

---

## 7. Practical Example

### Example: Adding New Status Value

**If we need to add a new status value (e.g., "PARTIAL_PASS"):**

**Option A: Update Skill Only** (CORRECT)
1. Update Skill section 1.2 to include new status
2. Update Skill section 7 (template) with new status option
3. Update Skill section 8 (checklist) with validation rule
4. Agent automatically recognizes new status in next run
5. Test designers immediately know to use new status

**Option B: Update Both** (WRONG)
- Creates duplication
- Risk of mismatch between Skill and Agent
- Multiple places to maintain

**Result:** Always update Skill, Agent references Skill automatically

---

## 8. Transition Guide: Using Both Files

### For Test Designers
1. **Read Skill section 7** → Get template
2. **Use Skill section 8** → Validate your test
3. **Submit test in PR**
4. **Reviewer uses Skill section 8** → Checklist for review

### For Code Reviewers
1. **Read Skill section 8** → Download checklist
2. **Review each test against checklist**
3. **Check Skill section 2** → Verify all 18 fields
4. **Check Skill section 1** → Verify ID/status/category format
5. **Approve if all Skill rules met**

### For CI/CD Engineers
1. **Run Agent** → Executes all validations
2. **Agent references Skill** → No manual schema checking needed
3. **Agent generates reports** → Review results
4. **Agent enforces gates** → Release blocked if gates fail

### For Release Manager
1. **Check Agent output** → Review metrics
2. **Verify Gate 3** → P0 tests PASS, coverage >80%
3. **Approve release** → If all Agent gates pass

---

## 9. Files & Locations

### Skill
- **Location:** `.github/Skills/UT-test-design-rules/SKILL.md`
- **Size:** ~200 lines
- **Update Frequency:** When schema rules change
- **Version:** 1.0 (current)

### Agent
- **Location:** `.github/Agents/Agent.md`
- **Size:** ~286 lines (reduced from 486 due to Skill extraction)
- **Update Frequency:** When validation procedures change
- **Version:** 1.0 (current)

### Supporting Docs
- **Analysis:** `.github/Docs/Agent-Skill-Architecture-Analysis.md`
- **Differences:** `.github/Docs/Skill-Agent-Differences.md` (this file)
- **Prompt:** `.github/Prompts/UT-test-design_prompt.md`

---

## 10. Benefits of This Architecture

| Benefit | Why | Result |
|---------|-----|--------|
| **Separation of Concerns** | Skill = what, Agent = how | Clear roles, no confusion |
| **Reusability** | Skill used by multiple tools | Cost savings, consistency |
| **Maintainability** | Update rules once, all auto-aligned | Fewer bugs, easier changes |
| **Clarity** | Each file has single purpose | Easier to understand, modify |
| **Scalability** | Team can grow, specialize | New tools use same Skill |
| **Composability** | Can build new agents using Skill | Faster agent development |
| **Consistency** | Single source of truth | No schema duplication |
| **Onboarding** | New members read clear docs | Faster learning curve |

---

## 11. Summary Table

| Aspect | SKILL | AGENT |
|--------|-------|-------|
| **File** | `.github/Skills/UT-test-design-rules/SKILL.md` | `.github/Agents/Agent.md` |
| **Purpose** | Define valid test schema | Execute & validate tests |
| **Type** | Passive reference | Active workflow |
| **Users** | Designers, reviewers, tools | CI/CD, automation |
| **Update Triggers** | Schema rules change | Procedures change |
| **Reusability** | High (multiple consumers) | Specific (execution-focused) |
| **Content** | Rules, templates, examples | Procedures, gates, algorithms |
| **Size** | ~200 lines | ~286 lines |
| **Question Answered** | "What should this look like?" | "How do I run this?" |
| **Section Count** | 11 sections | 14 sections |
| **Examples** | Yes, extensive | Yes, procedural |
| **Checklist** | Yes (section 8) | Yes (section 13) |

---

## References

- **Skill File:** `.github/Skills/UT-test-design-rules/SKILL.md`
- **Agent File:** `.github/Agents/Agent.md`
- **UT Design Prompt:** `.github/Prompts/UT-test-design_prompt.md`
- **Architecture Analysis:** `.github/Docs/Agent-Skill-Architecture-Analysis.md`

**Version:** 1.0  
**Date:** 2026-09-29  
**Status:** Active  
**Owner:** Design Team
