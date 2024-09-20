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
    <div
      class="two-columns w-full h-full grid grid-cols-2"
      :class="props.layoutClass"
    >
      <div
        class="grid grid-cols-1 h-full place-content-start place-items-center my-auto"
        :class="props.class"
      >
        <slot name="left" />
      </div>
      <div
        class="grid grid-cols-1 h-full place-content-start place-items-center my-auto"
        :class="props.class"
      >
        <slot name="right" />
      </div>
    </div>
  </div>
</template>
