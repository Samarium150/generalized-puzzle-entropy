<script setup lang="ts">
const props = defineProps({
  class: {
    type: String,
  },
  layoutClass: {
    type: String,
  },
});
</script>

<template>
  <div class="slidev-layout">
    <div class="title" :class="props.layoutClass">
      <slot />
    </div>
    <div class="w-full h-full flex flex-row -mt-1" :class="props.layoutClass">
      <div
        class="flex-1 h-full grow place-content-start place-items-start"
        :class="props.class"
      >
        <slot name="left" />
      </div>
      <div
        class="shrink place-content-start place-items-center max-w-2/5"
        :class="props.class"
      >
        <slot name="right" />
      </div>
    </div>
  </div>
</template>
